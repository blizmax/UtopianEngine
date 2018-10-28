#include "vulkan/SSAOEffect.h"
#include "vulkan/handles/DescriptorSet.h"
#include "vulkan/handles/DescriptorSetLayout.h"
#include "vulkan/handles/PipelineLayout.h"
#include "vulkan/Renderer.h"
#include "vulkan/handles/Texture.h"
#include "vulkan/ShaderFactory.h"
#include "vulkan/handles/Pipeline3.h"
#include "vulkan/handles/CommandBuffer.h"
#include "vulkan/handles/ComputePipeline.h"
#include "vulkan/PipelineInterface.h"
#include "vulkan/Vertex.h"
#include "core/renderer/Light.h"
#include "core/renderer/SceneJobs.h"
#include <memory>

namespace Utopian::Vk
{
	SSAOEffect::SSAOEffect()
	{
	}

	void SSAOEffect::CreateDescriptorPool(Device* device)
	{

	}

	void SSAOEffect::CreateVertexDescription(Device* device)
	{

	}

	void SSAOEffect::CreatePipelineInterface(Device* device)
	{
		
	}

	void SSAOEffect::CreateDescriptorSets(Device* device)
	{

	}

	void SSAOEffect::CreatePipeline(Renderer* renderer)
	{
		SharedPtr<Shader> shader = gShaderManager().CreateShaderOnline("data/shaders/ssao/ssao.vert", "data/shaders/ssao/ssao.frag");

		mPipeline = std::make_shared<Pipeline3>(renderer->GetDevice(), renderer->GetRenderPass(), mVertexDescription, shader);
		mPipeline->GetPipeline()->rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		mPipeline->GetPipeline()->depthStencilState.depthTestEnable = VK_TRUE;
		mPipeline->Create();

		cameraBlock.Create(renderer->GetDevice(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		settingsBlock.Create(renderer->GetDevice(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		mPipeline->BindUniformBuffer("UBO", cameraBlock.GetDescriptor());
		mPipeline->BindUniformBuffer("UBO_settings", settingsBlock.GetDescriptor());
	}

	void SSAOEffect::UpdateMemory()
	{
		cameraBlock.UpdateMemory();
		settingsBlock.UpdateMemory();
	}

	Pipeline* SSAOEffect::GetPipeline()
	{
		return mPipeline->GetPipeline();
	}

	void SSAOEffect::BindGBuffer(Image* positionImage, Image* normalViewImage, Image* albedoImage, Sampler* sampler)
	{
		mPipeline->BindCombinedImage("positionSampler", positionImage, sampler);
		mPipeline->BindCombinedImage("normalSampler", normalViewImage, sampler);
		mPipeline->BindCombinedImage("albedoSampler", albedoImage, sampler);
	}

	void SSAOEffect::BindDescriptorSets(CommandBuffer* commandBuffer)
	{
		mPipeline->BindDescriptorSets(commandBuffer);
	}

	void SSAOEffect::SetRenderPass(RenderPass* renderPass)
	{
		mRenderPass = renderPass;
	}
	
	void SSAOEffect::SetCameraData(glm::mat4 view, glm::mat4 projection, glm::vec4 eyePos)
	{
		cameraBlock.data.view = view;
		cameraBlock.data.projection = projection;
		cameraBlock.data.eyePos = eyePos;
		UpdateMemory();
	}

	void SSAOEffect::SetSettings(float radius, float bias)
	{
		settingsBlock.data.radius = radius;
		settingsBlock.data.bias = bias;
		UpdateMemory();
	}
}