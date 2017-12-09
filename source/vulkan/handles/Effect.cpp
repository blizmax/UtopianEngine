#include "vulkan/Effect.h"
#include "vulkan/Renderer.h"
#include "vulkan/Device.h"

namespace Vulkan
{
	Effect::Effect()
	{

	}

	void Effect::Init(Renderer* renderer)
	{
		CreateDescriptorPool(renderer->GetDevice());
		CreateVertexDescription(renderer->GetDevice());
		CreatePipelineInterface(renderer->GetDevice());
		CreateDescriptorSets(renderer->GetDevice());
		CreatePipeline(renderer); // To access the shader manager.
	}

	void Effect::SetPipeline(uint32_t pipelineType)
	{
		mActivePipeline = pipelineType;
	}

	VkPipelineLayout Effect::GetPipelineLayout()
	{
		return mPipelineInterface.GetPipelineLayout();
	}

	DescriptorSetLayout* Effect::GetDescriptorSetLayout(uint32_t descriptorSet)
	{
		return mPipelineInterface.GetDescriptorSetLayout(descriptorSet);
	}

	Pipeline2* Effect::GetPipeline()
	{
		return mPipelines[mActivePipeline];
	}

	DescriptorPool* Effect::GetDescriptorPool()
	{
		return mDescriptorPool;
	}

	VertexDescription* Effect::GetVertexDescription()
	{
		return mVertexDescription;
	}
}