#pragma once

#include "core/renderer/BaseJob.h"
#include "vulkan/DeferredEffect.h"
#include "vulkan/ScreenQuadUi.h"

namespace Utopian
{
	class DeferredJob : public BaseJob
	{
	public:
		DeferredJob(Vk::Device* device, uint32_t width, uint32_t height);
		~DeferredJob();

		void Init(const std::vector<BaseJob*>& jobs, const GBuffer& gbuffer) override;
		void Render(const JobInput& jobInput) override;

		SharedPtr<Vk::BasicRenderTarget> renderTarget;
		SharedPtr<Vk::Sampler> depthSampler;
		SharedPtr<Vk::DeferredEffect> effect;
	private:
		SharedPtr<ScreenQuad> mScreenQuad;
	};
}
