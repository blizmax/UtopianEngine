#pragma once
#include <string>
#include <glm/glm.hpp>
#include "utility/Common.h"
#include "vulkan/VulkanInclude.h"
#include "vulkan/ShaderBuffer.h"
#include "core/Terrain.h"
#include "imgui/imgui.h"

namespace Utopian
{
	class Terrain;

	struct BrushSettings
	{
		enum Mode {
			HEIGHT = 0,
			BLEND = 1
		};

		enum Operation {
			ADD = 0,
			REMOVE = 1
		};

		enum BlendLayer {
			GRASS = 0,
			ROCK,
			DIRT
		};

		glm::vec2 position;
		float radius;
		float strength;
		Mode mode;
		Operation operation;
		BlendLayer blendLayer;
	};

	class TerrainTool
	{
	public:
		TerrainTool(const SharedPtr<Terrain>& terrain, Vk::Device* device);
		~TerrainTool();

		void Update();
		void RenderUi();

		void EffectRecompiledCallback(std::string name);

		void SetupBlendmapBrushEffect();
		void SetupHeightmapBrushEffect();

		void RenderBlendmapBrush();
		void RenderHeightmapBrush();

	private:
		void UpdateBrushUniform();

	private:
		Vk::Device* mDevice;
		SharedPtr<Terrain> mTerrain;
		SharedPtr<Vk::Effect> mBlendmapBrushEffect;
		SharedPtr<Vk::Effect> mHeightmapBrushEffect;
		SharedPtr<Vk::RenderTarget> heightmapBrushRenderTarget;
		SharedPtr<Vk::RenderTarget> blendmapBrushRenderTarget;
		SharedPtr<Terrain::BrushBlock> brushBlock;
		BrushSettings brushSettings;
		SharedPtr<Vk::Texture2D> heightToolTexture;

		struct TextureIdentifiers
		{
			ImTextureID grass;
			ImTextureID rock;
			ImTextureID dirt;
			ImTextureID heightTool;
		} textureIdentifiers;
	};
}