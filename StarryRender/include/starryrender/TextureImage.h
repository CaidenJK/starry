#pragma once 

#include <StarryManager.h>
#include "ImageBuffer.h"

#include "stb_image.h"

namespace StarryRender
{
    class TextureImage : public ImageBuffer
    {
        public:
            TextureImage();
            ~TextureImage();
    
            void loadFromFile(const std::string filePath);

            enum SharedResources {
                IMAGE_VIEW = 0,
                SAMPLER = 1
            };

            std::optional<void*> getResource(size_t resourceID) override;
            size_t getResourceIDFromString(std::string resourceName) override;

            const std::string getAssetName() override { return "TextureImage"; }
        private:
            void loadImageToMemory(VkDeviceSize imageSize, stbi_uc* pixels);

            void createSampler();

            void transitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
            void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);

            int texWidth = 0;
            int texHeight = 0;
            int texChannels = 0;

            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
            
            VkSampler imageSampler = VK_NULL_HANDLE;
    };
}