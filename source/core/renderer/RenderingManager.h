#pragma once
#include <vector>
#include "vulkan/VulkanInclude.h"
#include "core/Object.h"
#include "utility/Module.h"
#include "utility/Common.h"
#include "vulkan/PhongEffect.h"
#include "vulkan/GBufferEffect.h"
#include "vulkan/DeferredEffect.h"
#include "vulkan/ColorEffect.h"
#include "core/CommonBuffers.h"
#include "core/renderer/SceneRenderers.h"
#include "vulkan/handles/DescriptorSetLayout.h"

class Terrain;

namespace Utopian
{
	class Renderable;
	class Light;
	class WaterRenderer;

	namespace Vk
	{
		class BasicRenderTarget;
	}

	class RenderingManager : public Module<RenderingManager>
	{
	public:
		RenderingManager(Vk::Renderer* renderer);
		~RenderingManager();

		void InitShaderResources();
		void InitShader();

		void Update();
		void RenderNodes(Vk::CommandBuffer* commandBuffer);
		void RenderScene(Vk::CommandBuffer* commandBuffer);
		void Render();
		void RenderOffscreen();
		void UpdateUniformBuffers();

		void AddRenderable(Renderable* renderable);
		void AddLight(Light* light);
		void AddCamera(Camera* camera);
		void SetMainCamera(Camera* camera);

		void SetTerrain(Terrain* terrain);
		void SetClippingPlane(glm::vec4 clippingPlane);

		void AddRenderer(BaseRenderer* renderer);

	private:
		SceneInfo mSceneInfo;
		Camera* mMainCamera;

		Vk::Renderer* mRenderer;
		Vk::CommandBuffer* mCommandBuffer;
		Vk::PhongEffect mPhongEffect;
		Vk::ColorEffect mColorEffect;
		Terrain* mTerrain;
		Vk::StaticModel* mCubeModel;
		WaterRenderer* mWaterRenderer;

		// Low level rendering 
		CameraUniformBuffer per_frame_vs;
		LightUniformBuffer per_frame_ps;
		FogUniformBuffer fog_ubo;
		Vk::DescriptorSetLayout mCommonDescriptorSetLayout;
		Vk::DescriptorSet* mCommonDescriptorSet;
		Vk::DescriptorPool* mCommonDescriptorPool;
		glm::vec4 mClippingPlane;

		std::map<uint32_t, Vk::Effect*> mEffects;

		/*  Deferred rendering experimentation */
		std::vector<BaseRenderer*> mRenderers;
		GBufferRenderer* mGBufferRenderer;
		DeferredRenderer* mDeferredRenderer;
		RenderingSettings mRenderingSettings;
	};
}
