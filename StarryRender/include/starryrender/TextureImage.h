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

            GET_RESOURCE;
            GET_RESOURCE_FROM_STRING;

            ASSET_NAME("TextureImage")
        private:
            void loadImageToMemory(VkDeviceSize imageSize, stbi_uc* pixels);

            void createSampler();

            int texWidth = 0;
            int texHeight = 0;
            int texChannels = 0;

            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
            
            VkSampler imageSampler = VK_NULL_HANDLE;
    };
}