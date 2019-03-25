#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "utility/Common.h"
#include "vulkan/VulkanInclude.h"
#include "vulkan/ShaderBuffer.h"
#include "core/Terrain.h"
#include "imgui/imgui.h"
#include <chrono>

namespace Utopian
{
	class Terrain;
	struct BrushSettings;

	class VegetationTool
	{
	public:
		VegetationTool(const SharedPtr<Terrain>& terrain, Vk::Device* device);
		~VegetationTool();

		// Uses brush settings from TerrainTool
		// Note: Todo: Remove this dependency
		void SetBrushSettings(BrushSettings* brushSettings);

		void Update();
		void RenderUi();

	private:
		void AddVegetation(uint32_t assetId, glm::vec3 position, bool animated, bool castShadows);
	private:
		Vk::Device* mDevice;
		SharedPtr<Terrain> mTerrain;
		BrushSettings* mBrushSettings;

		struct VegetationSettings
		{
			bool continuous;
			bool randomRotation;
			bool randomScale;
			float frequency;
			float minScale;
			float maxScale;
			uint32_t assetId;
		} mVegetationSettings;

		std::vector<const char*> mAssetNames;
		int mSelectedAsset;

		// Note: This should be handled by the Timer component.
		// But since the calls to ImGui::NewFrame() and ImGui::Render() currently are not 
		// called at the same frequency as UIOverlay::Update.
		std::chrono::high_resolution_clock::time_point mLastAddTimestamp;
	};
}