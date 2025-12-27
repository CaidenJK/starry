#include "ImageTexture.h"
#define STB_IMAGE_IMPLEMENTATION

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender
{
    ImageTexture::ImageTexture()
    {
        device = requestResource<VkDevice>("RenderDevice", "VkDevice");
    }

    ImageTexture::~ImageTexture()
    {
        
    }

    void ImageTexture::loadImageFromFile(std::string filePath)
    {
        ERROR_VOLATILE(loadFromFile(filePath.c_str()));
        //loadImageToMemory();
    }

    void ImageTexture::loadFromFile(const char* filePath)
    {
        pixels = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            registerAlert("Failed to load image from file!", CRITICAL);
        }
    }
}