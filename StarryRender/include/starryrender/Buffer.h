#pragma once

#include <StarryManager.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StarryRender
{
    class Buffer : public StarryAsset
    {
        public:
            Buffer();
            virtual ASSET_NAME("Buffer")

            virtual GET_RESOURCE { return {}; }
		    virtual GET_RESOURCE_FROM_STRING { return INVALID_RESOURCE; }
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