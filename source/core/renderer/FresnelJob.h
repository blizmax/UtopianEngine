#pragma once

#include "core/renderer/BaseJob.h"

namespace Utopian
{
	class FresnelJob : public BaseJob
	{
	public:
		FresnelJob(Vk::Device* device, uint32_t width, uint32_t height);
		~FresnelJob();

		void Init(const std::vector<BaseJob*>& jobs, const GBuffer& gbuffer) override;
		void Render(const JobInput& jobInput) override;

	private:
		SharedPtr<Vk::Effect> mEffect;
		SharedPtr<Vk::RenderTarget> mRenderTarget;
	};
}