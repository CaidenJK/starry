#include "ImageTexture.h"
#define STB_IMAGE_IMPLEMENTATION

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender
{
    ImageTexture::ImageTexture()
    {
    }

    ImageTexture::~ImageTexture()
    {
        
    }

    void ImageTexture::loadImageFromFile(std::string filePath)
    {
        ERROR_VOLATILE(loadFromFile(filePath.c_str()));
    }

    void ImageTexture::loadFromFile(const char* filePath)
    {
        pixels = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            registerAlert("Failed to load image from file!", CRITICAL);
        }

        loadImageToMemory(imageSize);
    }

    void ImageTexture::loadImageToMemory(VkDeviceSize imageSize)
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    }
}