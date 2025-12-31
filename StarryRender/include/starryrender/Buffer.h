#pragma once

#include <StarryManager.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace StarryRender
{
    class Buffer : public StarryAsset
    {
        public:
            Buffer();
            virtual const std::string getAssetName() override { return "Buffer"; }

            virtual std::optional<void*> getResource(size_t resourceID) override { return {}; }
		    virtual size_t getResourceIDFromString(std::string resourceName) override { return INVALID_RESOURCE; }
        protected:

            void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
			void copyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);
			uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

            VkCommandBuffer beginSingleTimeCommands();
            void endSingleTimeCommands(VkCommandBuffer& commandBuffer);
			
            ResourceHandle<VkDevice> device;
            ResourceHandle<VkPhysicalDevice> physicalDevice;
            ResourceHandle<VkCommandPool> commandPool;
            ResourceHandle<VkQueue> graphicsQueue;
    };
}