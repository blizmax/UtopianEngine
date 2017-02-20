#define GLM_FORCE_RADIANS
#define GLM_FORCE_RIGHT_HANDED 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/gtc/matrix_transform.hpp>
#include "ecs/Entity.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/TransformComponent.h"
#include "vulkan/VulkanApp.h"
#include "vulkan/handles/CommandBuffer.h"
#include "vulkan/handles/Pipeline.h"
#include "vulkan/handles/PipelineLayout.h"
#include "vulkan/handles/DescriptorSet.h"
#include "vulkan/Mesh.h"
#include "RenderSystem.h"
#include "Colors.h"

#define VERTEX_BUFFER_BIND_ID 0

namespace ECS
{
	RenderSystem::RenderSystem(EntityManager* entityManager, VulkanLib::VulkanApp* vulkanApp)
		: System(entityManager, Type::MESH_COMPONENT | Type::TRANSFORM_COMPONENT)
	{
		mVulkanApp = vulkanApp;
		mModelLoader = new VulkanLib::ModelLoader();

		mCubeModel = mModelLoader->LoadDebugBox(vulkanApp->GetDeviceTmp());

		AddDebugCube(vec3(0.0f, 0.0f, 0.0f), VulkanLib::Color::White, 70.0f);
		AddDebugCube(vec3(2000.0f, 0.0f, 0.0f), VulkanLib::Color::Red, 70.0f);
		AddDebugCube(vec3(0.0f, 2000.0f, 0.0f), VulkanLib::Color::Green, 70.0f);
		AddDebugCube(vec3(0.0f, 0.0f, 2000.0f), VulkanLib::Color::Blue, 70.0f);

		mCommandBuffer = mVulkanApp->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY);
	}

	RenderSystem::~RenderSystem()
	{
		mModelLoader->CleanupModels(mVulkanApp->GetDevice());
		delete mModelLoader;
	}

	void RenderSystem::Render(VulkanLib::CommandBuffer* commandBuffer, std::map<int, VulkanLib::Pipeline*>& pipelines, VulkanLib::PipelineLayout* pipelineLayout, VulkanLib::DescriptorSet& descriptorSet)
	{
	}

	void RenderSystem::OnEntityAdded(const EntityCache& entityCache)
	{
		VulkanLib::StaticModel* model = mModelLoader->LoadModel(mVulkanApp->GetDeviceTmp(), entityCache.meshComponent->GetFilename());
		entityCache.meshComponent->SetModel(model);
	}

	void RenderSystem::Process()
	{
		// Build mesh rendering command buffer
		mCommandBuffer->Begin(mVulkanApp->GetRenderPass(), mVulkanApp->GetCurrentFrameBuffer());

		mCommandBuffer->CmdSetViewPort(mVulkanApp->GetWindowWidth(), mVulkanApp->GetWindowHeight());
		mCommandBuffer->CmdSetScissor(mVulkanApp->GetWindowWidth(), mVulkanApp->GetWindowHeight());

		for (EntityCache entityCache : mEntities)
		{
			mCommandBuffer->CmdBindPipeline(mVulkanApp->GetPipeline(entityCache.meshComponent->GetPipeline()));
			mCommandBuffer->CmdBindDescriptorSet(mVulkanApp->GetPipelineLayout(), mVulkanApp->GetDescriptorSet());

			VulkanLib::StaticModel* model = entityCache.meshComponent->GetModel();

			// Push the world matrix constant
			VulkanLib::PushConstantBlock pushConstantBlock;
			pushConstantBlock.world = entityCache.transformComponent->GetWorldMatrix();
			pushConstantBlock.worldInvTranspose = entityCache.transformComponent->GetWorldInverseTransposeMatrix(); // TOOD: This probably also needs to be negated

			// NOTE: For some reason the translation needs to be negated when rendering
			// Otherwise the physical representation does not match the rendered scene
			pushConstantBlock.world[3][0] = -pushConstantBlock.world[3][0];	
			pushConstantBlock.world[3][1] = -pushConstantBlock.world[3][1];
			pushConstantBlock.world[3][2] = -pushConstantBlock.world[3][2];

			mCommandBuffer->CmdPushConstants(mVulkanApp->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstantBlock), &pushConstantBlock);

			mCommandBuffer->CmdBindVertexBuffer(VERTEX_BUFFER_BIND_ID, 1, &model->mMeshes[0]->vertices.buffer);
			mCommandBuffer->CmdBindIndexBuffer(model->mMeshes[0]->indices.buffer, 0, VK_INDEX_TYPE_UINT32);
			mCommandBuffer->CmdDrawIndexed(model->GetNumIndices(), 1, 0, 0, 0);
		}

		// Draw debug boxes
		for (EntityCache entityCache : mEntities)
		{
			mCommandBuffer->CmdBindPipeline(mVulkanApp->GetPipeline(VulkanLib::PipelineType::PIPELINE_TEST));
			mCommandBuffer->CmdBindDescriptorSet(mVulkanApp->GetPipelineLayout(), mVulkanApp->GetDescriptorSet());

			VulkanLib::BoundingBox meshBoundingBox = entityCache.meshComponent->GetBoundingBox();
			meshBoundingBox.Update(entityCache.transformComponent->GetWorldMatrix()); 

			mat4 world = mat4();
			float width = meshBoundingBox.GetWidth();
			float height = meshBoundingBox.GetHeight();
			float depth = meshBoundingBox.GetDepth();
			world = glm::translate(world, -entityCache.transformComponent->GetPosition());
			world = glm::scale(world, glm::vec3(width, height, depth));

			PushConstantDebugBlock pushConstantBlock;
			pushConstantBlock.world = world;
			pushConstantBlock.color = VulkanLib::Color::White;

			mCommandBuffer->CmdPushConstants(mVulkanApp->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstantBlock), &pushConstantBlock);
			mCommandBuffer->CmdBindVertexBuffer(VERTEX_BUFFER_BIND_ID, 1, &mCubeModel->mMeshes[0]->vertices.buffer);
			mCommandBuffer->CmdBindIndexBuffer(mCubeModel->mMeshes[0]->indices.buffer, 0, VK_INDEX_TYPE_UINT32);
			mCommandBuffer->CmdDrawIndexed(mCubeModel->GetNumIndices(), 1, 0, 0, 0);
		}

		mCommandBuffer->CmdBindPipeline(mVulkanApp->GetPipeline(VulkanLib::PipelineType::PIPELINE_DEBUG));
		mCommandBuffer->CmdBindDescriptorSet(mVulkanApp->GetPipelineLayout(), mVulkanApp->GetDescriptorSet());

		// Draw debug cubes for the origin and each axis
		for (int i = 0; i < mDebugCubes.size(); i++)
		{
			DebugCube debugCube = mDebugCubes[i];

			glm::mat4 world = mat4();
			world = glm::translate(world, -debugCube.pos);
			world = glm::scale(world, glm::vec3(debugCube.size));

			PushConstantDebugBlock pushConstantBlock;
			pushConstantBlock.world = world;
			pushConstantBlock.color = debugCube.color;

			mCommandBuffer->CmdPushConstants(mVulkanApp->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstantBlock), &pushConstantBlock);
			mCommandBuffer->CmdBindVertexBuffer(VERTEX_BUFFER_BIND_ID, 1, &mCubeModel->mMeshes[0]->vertices.buffer);
			mCommandBuffer->CmdBindIndexBuffer(mCubeModel->mMeshes[0]->indices.buffer, 0, VK_INDEX_TYPE_UINT32);
			mCommandBuffer->CmdDrawIndexed(mCubeModel->GetNumIndices(), 1, 0, 0, 0);
		}

		mCommandBuffer->End();
	}

	void RenderSystem::AddDebugCube(vec3 pos, vec4 color, float size)
	{
		mDebugCubes.push_back(DebugCube(pos, color, size));
	}

	void RenderSystem::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

	}
}