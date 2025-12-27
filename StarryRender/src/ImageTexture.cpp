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
        if (pixels != nullptr) stbi_image_free(pixels);
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
        createImageObject(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
    }

    void ImageTexture::loadImageToMemory(VkDeviceSize imageSize)
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        if (device.wait() != ResourceState::YES) {
			registerAlert("Device died before it was ready to be used.", FATAL);
		}

        void* data;
        vkMapMemory(*device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(*device, stagingBufferMemory);
    }

    void ImageTexture::createImageObject(uint32_t width, uint32_t height, VkFormat format, 
        VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
        VkImage& image, VkDeviceMemory& imageMemory)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (device.wait() != ResourceState::YES) {
			registerAlert("Device died before it was ready to be used.", FATAL);
		}

        if (vkCreateImage(*device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            registerAlert("Failed to create image.", CRITICAL);
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(*device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(*device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(*device, image, imageMemory, 0);
    }

    void ImageTexture::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        endSingleTimeCommands(commandBuffer);
    }
}