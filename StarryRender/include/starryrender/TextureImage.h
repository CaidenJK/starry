#pragma once 

#include <StarryManager.h>
#include "ImageBuffer.h"

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
            void loadImageToMemory(VkDeviceSize imageSize, ImageFile* file);

            void createSampler();

            ResourceHandle<FILETYPE> file;

            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
            
            VkSampler imageSampler = VK_NULL_HANDLE;
    };
}