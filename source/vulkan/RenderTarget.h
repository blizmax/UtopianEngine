#pragma once

#include <stdint.h>
#include <glm/glm.hpp>
#include <vector>
#include "vulkan/VulkanPrerequisites.h"
#include "utility/Common.h"

namespace Utopian::Vk
{
	class RenderTarget
	{
	public:
		RenderTarget(Device* device, uint32_t width, uint32_t height);
		~RenderTarget();

		/** Begins the command buffer and the render pass. */
		void Begin(std::string debugName = "Unnamed pass", glm::vec4 debugColor = glm::vec4(1.0, 0.0, 0.0, 1.0));

		void BeginCommandBuffer(std::string debugName = "Unnamed pass", glm::vec4 debugColor = glm::vec4(1.0, 0.0, 0.0, 1.0));
		void BeginRenderPass();

		// Special version that instead of using the framebuffer in RenderTarget
		// will use the supplied one. 
		// Note: Assumes that the renderpass and framebuffers are compatible.
		// Todo: Note: Does not begin the command buffer, has to be done before calling this.
		void Begin(VkFramebuffer framebuffer, std::string debugName = "Unnamed pass", glm::vec4 debugColor = glm::vec4(1.0, 0.0, 0.0, 1.0));
		void End();
		void End(const SharedPtr<Semaphore>& waitSemaphore, const SharedPtr<Semaphore>& signalSemaphore);

		void SetClearColor(float r, float g, float b, float a = 0.0f);

		void AddReadWriteColorAttachment(const SharedPtr<Image>& image,
		 								 VkImageLayout finalImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		 								 VkImageLayout initialImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		void AddWriteOnlyColorAttachment(const SharedPtr<Image>& image,
		 								 VkImageLayout finalImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		 								 VkImageLayout initialImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		void AddReadWriteDepthAttachment(const SharedPtr<Image>& image,
										 VkImageLayout finalImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
										 VkImageLayout initialImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		void AddWriteOnlyDepthAttachment(const SharedPtr<Image>& image,
										 VkImageLayout finalImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
										 VkImageLayout initialImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		// Some images can have multiple views (array layers > 1) so in those cases
		// you must provide the exact VkImageView
		void AddColorAttachment(VkImageView imageView,
								VkFormat format,
								VkImageLayout finalImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
								VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
								VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE);

		void AddDepthAttachment(VkImageView imageView,
								VkFormat format,
								VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
								VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE);

		void Create();

		Utopian::Vk::Sampler* GetSampler();
		Utopian::Vk::CommandBuffer* GetCommandBuffer();
		RenderPass* GetRenderPass();

		uint32_t GetWidth();
		uint32_t GetHeight();

	private:
		SharedPtr<FrameBuffers> mFrameBuffer;
		SharedPtr<RenderPass> mRenderPass;
		SharedPtr<CommandBuffer> mCommandBuffer;
		SharedPtr<Sampler> mSampler;
		uint32_t mWidth, mHeight;
		glm::vec4 mClearColor;
		std::vector<VkClearValue> mClearValues;
	};
}
