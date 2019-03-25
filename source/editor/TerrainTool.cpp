#include "editor/TerrainTool.h"
#include "vulkan/EffectManager.h"
#include "vulkan/RenderTarget.h"
#include "vulkan/Effect.h"
#include "vulkan/handles/CommandBuffer.h"
#include "vulkan/handles/Image.h"
#include "vulkan/UIOverlay.h"
#include "vulkan/Texture2.h"
#include "core/renderer/Renderer.h"
#include "core/renderer/RendererUtility.h"
#include "core/Terrain.h"
#include "Input.h"
#include "Camera.h"

namespace Utopian
{
   TerrainTool::TerrainTool(const SharedPtr<Terrain>& terrain, Vk::Device* device)
   {
		mTerrain = terrain;
		mDevice = device;

		brushBlock = terrain->GetBrushBlock();

		SetupBlendmapBrushEffect();
		SetupHeightmapBrushEffect();

		RenderBlendmapBrush();
		RenderHeightmapBrush();

		Vk::gEffectManager().RegisterRecompileCallback(&TerrainTool::EffectRecompiledCallback, this);

		// Temporary:
		brushSettings.mode = BrushSettings::Mode::BLEND;
		brushSettings.operation = BrushSettings::Operation::ADD;
		brushSettings.blendLayer = BrushSettings::BlendLayer::GRASS;
		brushSettings.strength = 240.0f;
		brushSettings.radius = 500.0f;

		heightToolTexture = std::make_shared<Vk::Texture2D>("data/textures/height-tool.ktx", mDevice);

		Vk::UIOverlay* uiOverlay = gRenderer().GetUiOverlay();
		textureIdentifiers.grass = uiOverlay->AddTexture(mTerrain->GetMaterial("grass").diffuse->view, mTerrain->GetMaterial("grass").diffuse->sampler);
		textureIdentifiers.rock = uiOverlay->AddTexture(mTerrain->GetMaterial("rock").diffuse->view, mTerrain->GetMaterial("rock").diffuse->sampler);
		textureIdentifiers.dirt = uiOverlay->AddTexture(mTerrain->GetMaterial("dirt").diffuse->view, mTerrain->GetMaterial("dirt").diffuse->sampler);
		textureIdentifiers.heightTool = uiOverlay->AddTexture(heightToolTexture->view);
   }

   TerrainTool::~TerrainTool()
   {

   }

   void TerrainTool::Update()
   {
		RenderUi();
		glm::vec3 cameraPos = gRenderer().GetMainCamera()->GetPosition();
		static glm::vec3 intersection = glm::vec3(0.0);

		Ray ray = gRenderer().GetMainCamera()->GetPickingRay();
		intersection = mTerrain->GetIntersectPoint(ray);
		brushSettings.position = mTerrain->TransformToUv(intersection.x, intersection.z);
		brushSettings.radius += gInput().MouseDz() / 4.0f;

		UpdateBrushUniform();
		
		if (brushSettings.mode == BrushSettings::Mode::BLEND)
		{
			if (gInput().KeyDown(VK_LBUTTON))
			{
				RenderBlendmapBrush();
			}
		}
		else if (brushSettings.mode == BrushSettings::Mode::HEIGHT)
		{
			if (gInput().KeyDown(VK_LBUTTON) || gInput().KeyDown(VK_RBUTTON))
			{
				if (gInput().KeyDown(VK_LBUTTON))
					brushSettings.operation = BrushSettings::Operation::ADD;
				else
					brushSettings.operation = BrushSettings::Operation::REMOVE;

				RenderHeightmapBrush();
				mTerrain->RenderNormalmap();
				mTerrain->RenderBlendmap();
				mTerrain->RetrieveHeightmap();
			}
		}
   }

   void TerrainTool::RenderUi()
   {
	   // Display Actor creation list
	   Vk::UIOverlay::BeginWindow("Terrain tool", glm::vec2(1500.0f, 1050.0f), 200.0f);

	   ImGui::SliderFloat("Brush radius", &brushSettings.radius, 0.0f, 10000.0f);
	   ImGui::SliderFloat("Brush strenth", &brushSettings.strength, 0.0f, 299.0f);

	   if (ImGui::ImageButton(textureIdentifiers.heightTool, ImVec2(64, 64)))
	   {
		   brushSettings.mode = BrushSettings::Mode::HEIGHT;
	   }

	   ImGui::SameLine();

	   if (ImGui::ImageButton(textureIdentifiers.grass, ImVec2(64, 64)))
	   {
		   brushSettings.mode = BrushSettings::Mode::BLEND;
		   brushSettings.blendLayer = BrushSettings::BlendLayer::GRASS;
	   }

	   ImGui::SameLine();

	   if (ImGui::ImageButton(textureIdentifiers.rock, ImVec2(64, 64)))
	   {
		   brushSettings.mode = BrushSettings::Mode::BLEND;
		   brushSettings.blendLayer = BrushSettings::BlendLayer::ROCK;
	   }

	   ImGui::SameLine();

	   if (ImGui::ImageButton(textureIdentifiers.dirt, ImVec2(64, 64)))
	   {
		   brushSettings.mode = BrushSettings::Mode::BLEND;
		   brushSettings.blendLayer = BrushSettings::BlendLayer::DIRT;
	   }

	   Vk::UIOverlay::EndWindow();
   }

	void TerrainTool::EffectRecompiledCallback(std::string name)
	{
		RenderBlendmapBrush();
		RenderHeightmapBrush();

		//mTerrain->RetrieveHeightmap();
	}

   void TerrainTool::SetupBlendmapBrushEffect()
	{
		blendmapBrushRenderTarget = std::make_shared<Vk::RenderTarget>(mDevice, 256, 256);
		blendmapBrushRenderTarget->AddReadWriteColorAttachment(mTerrain->GetBlendmapImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL);
		blendmapBrushRenderTarget->SetClearColor(1, 1, 1, 1);
		blendmapBrushRenderTarget->Create();

		Vk::ShaderCreateInfo shaderCreateInfo;
		shaderCreateInfo.vertexShaderPath = "data/shaders/common/fullscreen.vert";
		shaderCreateInfo.fragmentShaderPath = "data/shaders/tessellation/blendmap_brush.frag";
		mBlendmapBrushEffect = Vk::gEffectManager().AddEffect<Vk::Effect>(mDevice, blendmapBrushRenderTarget->GetRenderPass(), shaderCreateInfo);

		// Vertices generated in fullscreen.vert are in clockwise order
		mBlendmapBrushEffect->GetPipeline()->rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
		mBlendmapBrushEffect->GetPipeline()->rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		mBlendmapBrushEffect->CreatePipeline();

		mBlendmapBrushEffect->BindUniformBuffer("UBO_brush", brushBlock.get());
	}

	void TerrainTool::SetupHeightmapBrushEffect()
	{
		heightmapBrushRenderTarget = std::make_shared<Vk::RenderTarget>(mDevice, mTerrain->GetMapResolution(), mTerrain->GetMapResolution());
		heightmapBrushRenderTarget->AddReadWriteColorAttachment(mTerrain->GetHeightmapImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL);
		heightmapBrushRenderTarget->SetClearColor(1, 1, 1, 1);
		heightmapBrushRenderTarget->Create();

		Vk::ShaderCreateInfo shaderCreateInfo;
		shaderCreateInfo.vertexShaderPath = "data/shaders/common/fullscreen.vert";
		shaderCreateInfo.fragmentShaderPath = "data/shaders/tessellation/heightmap_brush.frag";
		mHeightmapBrushEffect = Vk::gEffectManager().AddEffect<Vk::Effect>(mDevice, heightmapBrushRenderTarget->GetRenderPass(), shaderCreateInfo);

		// Vertices generated in fullscreen.vert are in clockwise order
		mHeightmapBrushEffect->GetPipeline()->rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
		mHeightmapBrushEffect->GetPipeline()->rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		gRendererUtility().SetAdditiveBlending(mHeightmapBrushEffect->GetPipeline());

		mHeightmapBrushEffect->CreatePipeline();

		mHeightmapBrushEffect->BindUniformBuffer("UBO_brush", brushBlock.get());
	}

	void TerrainTool::UpdateBrushUniform()
	{
		brushBlock->data.brushPos = brushSettings.position;
		brushBlock->data.radius = brushSettings.radius / mTerrain->GetTerrainSize();
		brushBlock->data.strength = brushSettings.strength;
		brushBlock->data.mode = brushSettings.mode;
		brushBlock->data.operation = brushSettings.operation;
		brushBlock->data.blendLayer = brushSettings.blendLayer;
		brushBlock->UpdateMemory();
	}

   void TerrainTool::RenderBlendmapBrush()
	{
		blendmapBrushRenderTarget->Begin("Blendmap brush pass");
		Vk::CommandBuffer* commandBuffer = blendmapBrushRenderTarget->GetCommandBuffer();
		commandBuffer->CmdBindPipeline(mBlendmapBrushEffect->GetPipeline());
		commandBuffer->CmdBindDescriptorSets(mBlendmapBrushEffect);
		gRendererUtility().DrawFullscreenQuad(commandBuffer);
		blendmapBrushRenderTarget->End();
	}

	void TerrainTool::RenderHeightmapBrush()
	{
		heightmapBrushRenderTarget->Begin("Heightmap brush pass");
		Vk::CommandBuffer* commandBuffer = heightmapBrushRenderTarget->GetCommandBuffer();
		commandBuffer->CmdBindPipeline(mHeightmapBrushEffect->GetPipeline());
		commandBuffer->CmdBindDescriptorSets(mHeightmapBrushEffect);
		gRendererUtility().DrawFullscreenQuad(commandBuffer);
		heightmapBrushRenderTarget->End();
	}

	BrushSettings* TerrainTool::GetBrushSettings()
	{
		return &brushSettings;
	}
}