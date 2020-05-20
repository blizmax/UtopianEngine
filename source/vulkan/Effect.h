#pragma once

#include <array>
#include <vector>
#include <map>
#include <string>
#include "vulkan/VulkanInclude.h"
#include "vulkan/VertexDescription.h"
#include "vulkan/handles/DescriptorSetLayout.h"
#include "vulkan/handles/DescriptorSet.h"
#include "vulkan/handles/Pipeline.h"
#include "vulkan/PipelineInterface.h"
#include "vulkan/ShaderFactory.h"
#include "utility/Common.h"

namespace Utopian::Vk
{
	class Effect
	{
	public:
		/** @note CreatePipeline() must be called explicitly after constructor. */
		Effect(Device* device, RenderPass* renderPass);
		Effect(Device* device, RenderPass* renderPass, const ShaderCreateInfo& shaderCreateInfo);

		/** Loads the shaders from file again, compiles it, performs reflection and rebuilds the pipeline. */
		void RecompileShader();

		/**
		 * This must explicitly be called. Reason being that the constructor sets default values and to make
		 * modifications to the pipeline they must be made between before calling this function.
		 */
		void CreatePipeline();

		/** 
		 * Functions used to bind shader resources by name.
		 * @note the name must match the name in the GLSL shader.
		 * @note uses the internal shader reflection to perform name -> set ID mapping.
		 */
		void BindUniformBuffer(std::string name, const VkDescriptorBufferInfo* bufferInfo);
		void BindStorageBuffer(std::string name, const VkDescriptorBufferInfo* bufferInfo);
		void BindUniformBuffer(std::string name, const ShaderBuffer& shaderBlock);
		void BindCombinedImage(std::string name, const Texture& texture);
		void BindCombinedImage(std::string name, const Image& image, const Sampler& sampler);
		void BindCombinedImage(std::string name, const TextureArray& textureArray);

		/** 
		 * Returns a descriptor set by index.
		 * Needs to be used if you want to bind additional descriptor sets that not are part of the Effect itself,
		 * for example Meshes contains their own descriptor set for their texture.
		 * @note This should only be used in rare cases.
		 */
		const DescriptorSet& GetDescriptorSet(uint32_t set) const;

		/** Returns a pointer to the Pipeline object. It is expected to be modified. */
		Pipeline* GetPipeline();

		const VkDescriptorSet* GetDescriptorSets() const;
		ShaderCreateInfo GetShaderCreateInfo() const;
		std::string GetVertexShaderPath() const;
		uint32_t GetNumDescriptorSets() const;
		SharedPtr<Shader> GetShader() const;
		PipelineInterface* GetPipelineInterface();
	protected:
		void SetShaderCreateInfo(const ShaderCreateInfo& shaderCreateInfo);

		SharedPtr<Pipeline> mPipeline;
	private:
		void Init();
		void CreatePipelineInterface(const SharedPtr<Shader>& shader, Device* device);

		RenderPass* mRenderPass = nullptr;
		Device* mDevice = nullptr;
		SharedPtr<Shader> mShader;
		SharedPtr<PipelineInterface> mPipelineInterface;
		std::vector<DescriptorSet> mDescriptorSets;
		std::vector<VkDescriptorSet> mVkDescriptorSets;
		ShaderCreateInfo mShaderCreateInfo;
		SharedPtr<DescriptorPool> mDescriptorPool;
	};
}
