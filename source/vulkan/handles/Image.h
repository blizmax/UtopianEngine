#pragma once

#include "Handle.h"
#include "vulkan/VulkanInclude.h"
#include <vector>

namespace Utopian::Vk
{
	/** Wrapper for VkImage and VkImageView. */
	class Image : public Handle<VkImage>
	{
	public:
		/** Constructor that should be used in most cases. */
		Image(Device* device,
			  uint32_t width,
			  uint32_t height,
			  VkFormat format,
			  VkImageTiling tiling,
			  VkImageUsageFlags usage,
			  VkMemoryPropertyFlags properties,
			  VkImageAspectFlags aspectFlags,
			  uint32_t arrayLayers = 1);

		/** If specialized create infos are needed this should be called followed by CreateImage() and CreateView(). */
		Image(Device* device);

		~Image();

		/**
		 * These exist so that it's possible to create images and view with non standard create infos
		 * For example used by CubeMapTexture.
		 */
		void CreateImage(VkImageCreateInfo imageCreateInfo, VkMemoryPropertyFlags properties);
		void CreateView(VkImageViewCreateInfo viewCreateInfo);

		VkImageView GetView() const;
		VkImageView GetLayerView(uint32_t layer) const;
		VkFormat GetFormat() const;
	private:
		/** If the image has multiple layers this contains the view to each one of them. */
		std::vector<VkImageView> mLayerViews;

		/** Contains the view to the whole image, including all layers if more than one. */
		VkImageView mImageView;

		VkDeviceMemory mDeviceMemory;
		VkFormat mFormat;
	};

	/** An image with flags corresponding to a color image. */
	class ImageColor : public Image
	{
	public:
		ImageColor(Device* device, uint32_t width, uint32_t height, VkFormat format, uint32_t arrayLayers = 1);
	};

	/** An image with flags corresponding to a depth image. */
	class ImageDepth : public Image
	{
	public:
		ImageDepth(Device* device, uint32_t width, uint32_t height, VkFormat format, uint32_t arrayLayers = 1);
	};
}
