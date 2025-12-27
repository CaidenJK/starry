#pragma once

#include <StarryAsset.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace StarryRender
{
    class Buffer : public StarryAsset
    {
        public:
            Buffer();
            virtual const std::string getAssetName() override { return "Buffer"; }
        protected:

            void createBuffer(VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
			void copyBuffer(VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);
			uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
			
            ResourceHandle<VkDevice> device;
    };
}