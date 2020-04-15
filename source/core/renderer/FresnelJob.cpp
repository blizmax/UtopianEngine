#include "core/renderer/FresnelJob.h"
#include "core/renderer/CommonJobIncludes.h"
#include "core/renderer/DeferredJob.h"
#include "core/renderer/SSRJob.h"
#include "core/renderer/OpaqueCopyJob.h"

namespace Utopian
{
	FresnelJob::FresnelJob(Vk::Device* device, uint32_t width, uint32_t height)
		: BaseJob(device, width, height)
	{
	}

	FresnelJob::~FresnelJob()
	{
	}

	void FresnelJob::Init(const std::vector<BaseJob*>& jobs, const GBuffer& gbuffer)
	{
		DeferredJob* deferredJob = static_cast<DeferredJob*>(jobs[JobGraph::DEFERRED_INDEX]);
		SSRJob* ssrJob = static_cast<SSRJob*>(jobs[JobGraph::SSR_INDEX]);
		OpaqueCopyJob* opaqueCopyJob = static_cast<OpaqueCopyJob*>(jobs[JobGraph::OPAQUE_COPY_INDEX]);

		mRenderTarget = std::make_shared<Vk::RenderTarget>(mDevice, mWidth, mHeight);
		mRenderTarget->AddReadWriteColorAttachment(deferredJob->renderTarget->GetColorImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		mRenderTarget->Create();

		Vk::ShaderCreateInfo shaderCreateInfo;
		shaderCreateInfo.vertexShaderPath = "data/shaders/common/fullscreen.vert";
		shaderCreateInfo.fragmentShaderPath = "data/shaders/post_process/fresnel.frag";

		mEffect = Vk::gEffectManager().AddEffect<Vk::Effect>(mDevice, mRenderTarget->GetRenderPass(), shaderCreateInfo);

		// Vertices generated in fullscreen.vert are in clockwise order
		mEffect->GetPipeline()->rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
		mEffect->GetPipeline()->rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		gRendererUtility().SetAdditiveBlending(mEffect->GetPipeline());

		mEffect->CreatePipeline();

		mEffect->BindCombinedImage("reflectionSampler", ssrJob->ssrBlurImage.get(), mRenderTarget->GetSampler());
		mEffect->BindCombinedImage("refractionSampler", opaqueCopyJob->opaqueLitImage.get(), mRenderTarget->GetSampler());
	}

	void FresnelJob::Render(const JobInput& jobInput)
	{
		mRenderTarget->Begin("Fresnel pass", glm::vec4(0.5, 1.0, 0.5, 1.0));
		Vk::CommandBuffer* commandBuffer = mRenderTarget->GetCommandBuffer();

		if (IsEnabled())
		{
			commandBuffer->CmdBindPipeline(mEffect->GetPipeline());
			commandBuffer->CmdBindDescriptorSets(mEffect);
			gRendererUtility().DrawFullscreenQuad(commandBuffer);
		}

		mRenderTarget->End(GetWaitSemahore(), GetCompletedSemahore());
	}
}