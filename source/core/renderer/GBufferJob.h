#pragma once

#include "core/renderer/BaseJob.h"
#include "vulkan/GBufferEffect.h"

namespace Utopian
{
	class GBufferJob : public BaseJob
	{
	public:
		UNIFORM_BLOCK_BEGIN(GBufferViewProjection)
			UNIFORM_PARAM(glm::mat4, projection)
			UNIFORM_PARAM(glm::mat4, view)
		UNIFORM_BLOCK_END()

		UNIFORM_BLOCK_BEGIN(SettingsBlock)
			UNIFORM_PARAM(int, normalMapping)
		UNIFORM_BLOCK_END()

		GBufferJob(Vk::Device* device, uint32_t width, uint32_t height);
		~GBufferJob();

		void Init(const std::vector<BaseJob*>& jobs, const GBuffer& gbuffer) override;
		void Render(const JobInput& jobInput) override;

		SharedPtr<Vk::RenderTarget> renderTarget;

		SharedPtr<Vk::GBufferEffect> mGBufferEffect;
		SharedPtr<Vk::GBufferEffect> mGBufferEffectWireframe;
		SharedPtr<Vk::Effect> mGBufferEffectTerrain;
		SharedPtr<Vk::Effect> mGBufferEffectInstanced;
	private:
		GBufferViewProjection viewProjectionBlock;
		SettingsBlock settingsBlock;
		SharedPtr<Vk::Sampler> sampler;
	};
}
