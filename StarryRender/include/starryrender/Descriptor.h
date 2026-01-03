#pragma once 

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <StarryManager.h>

#include <vector>

#define MAX_FRAMES_IN_FLIGHT 2

namespace StarryRender
{
    class Descriptor : public StarryAsset
    {
        public:
            Descriptor();
            ~Descriptor();
            
            VkDescriptorSetLayout& getDescriptorSetLayout() { return descriptorSetLayout; }
		    VkDescriptorSet& getDescriptorSet(uint32_t index) { return descriptorSets[index]; }

            void createSets(uint64_t uniformBufferUUID, uint64_t imageTextureID); // Scale later

            ASSET_NAME("Descriptor")
        private:
            void createDescriptorSetLayout();
            void createDescriptorPool();
		    void createDescriptorSets(std::vector<VkBuffer>& uniformBuffers, VkImageView& textureImageView, VkSampler& textureImageSampler);

            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

		    std::vector<VkDescriptorSet> descriptorSets;

            ResourceHandle<VkDevice> device;
    };
}