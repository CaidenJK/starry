#pragma once

#include <StarryAsset.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace StarryRender
{
	class Buffer : public StarryAsset
	{
		public:
			virtual const std::string getAssetName() { return "Buffer"; }
		protected:
			Buffer();

			virtual void createBuffer(VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
			virtual void copyBuffer(VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);

			virtual uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

			ResourceHandle<VkDevice> device{};
	};
}