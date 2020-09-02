#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "vulkan/VulkanPrerequisites.h"
#include "../external/vk_mem_alloc.h"

namespace Utopian::Vk
{
	struct VulkanVersion
	{
		VulkanVersion();
		VulkanVersion(uint32_t apiVersion);

		uint32_t major;
		uint32_t minor;
		uint32_t patch;
		std::string version;
	};

	/** Wrapper for the Vulkan device. */
	class Device
	{
	public:
		Device(Instance* instance, bool enableValidation = false);
		~Device();

		/**
		 * Returns the graphics queue associated with the device.
		 * 
		 * @note Currently only one queue is fetched from the device.
		 */
		Queue* GetQueue() const;

		/* Memory management. */
		VmaAllocation AllocateMemory(Image* image, VkMemoryPropertyFlags flags);
		VmaAllocation AllocateMemory(Buffer* buffer, VkMemoryPropertyFlags flags);
		void MapMemory(VmaAllocation allocation, void** data);
		void UnmapMemory(VmaAllocation allocation);
		void FreeMemory(VmaAllocation allocation);
		void GetAllocationInfo(VmaAllocation allocation, VkDeviceMemory& memory, VkDeviceSize& offset);

		/** Returns the combined memory budget for all heaps matching heapFlags. */
		VmaBudget GetMemoryBudget(VkMemoryHeapFlags heapFlags);

		/** Returns detailed memory statistics. */
		void GetMemoryStats(VmaStats* stats);

		/** Writes memory statistics to .json file, can be visualized with VmpaDumpVis.py. */
		void DumpMemoryStats(std::string filename);

		/* Returns device memory properties. */
		VkPhysicalDeviceMemoryProperties GetMemoryProperties() const;

		/** Returns the command pool from the device which new command buffers can be allocated from. */
		CommandPool* GetCommandPool() const;

		VkPhysicalDevice GetPhysicalDevice() const;
		VkDevice GetVkDevice() const;
		uint32_t GetMemoryType(uint32_t typeBits, VkFlags properties, uint32_t * typeIndex) const;
		bool IsDebugMarkersEnabled() const;
		uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags) const;
		VulkanVersion GetVulkanVersion() const;

	private:
		void RetrievePhysical(Instance* instance);
		void RetrieveQueueFamilyProperites();
		void CreateLogical(bool enableValidation);
		void RetrieveSupportedExtensions();
		bool IsExtensionSupported(std::string extension);

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties mPhysicalDeviceProperties;
		VkPhysicalDeviceMemoryProperties mDeviceMemoryProperties;
		VkPhysicalDeviceFeatures mEnabledFeatures {};
		VkPhysicalDeviceFeatures mAvailableFeatures;
		std::vector<std::string> mSupportedExtensions;
		std::vector<VkQueueFamilyProperties> mQueueFamilyProperties;
		VulkanVersion mVulkanVersion;
		VmaAllocator mAllocator;

		CommandPool* mCommandPool = nullptr;
		Queue* mQueue = nullptr;
		bool mDebugMarkersEnabled = false;
	};
}