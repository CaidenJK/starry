#include "TextureImage.h"

namespace StarryRender
{
    TextureImage::TextureImage()
    {
        device = requestResource<VkDevice>("Render Device", "VkDevice");
    }

    TextureImage::~TextureImage()
    {
        if (device) {
            if (imageSampler != VK_NULL_HANDLE) vkDestroySampler(*device, imageSampler, nullptr);
        }
    }

    std::optional<void*> TextureImage::getResource(size_t resourceID, std::vector<size_t> resourceArgs)
    {
        if (resourceID == SharedResources::IMAGE_VIEW &&
            imageView != VK_NULL_HANDLE) {
            return (void*)&imageView;
        }

        if (resourceID == SharedResources::SAMPLER &&
            imageSampler != VK_NULL_HANDLE) {
            return (void*)&imageSampler;
        }

        registerAlert(std::string("No matching resource: ") + std::to_string(resourceID) + " available for sharing", WARNING);
		return {};
    }

	size_t TextureImage::getResourceIDFromString(const std::string resourceName)
    {
        if (resourceName.compare("Image View") == 0) {
            return SharedResources::IMAGE_VIEW;
        }
        if (resourceName.compare("Sampler") == 0) {
            return SharedResources::SAMPLER;
        }

        return INVALID_RESOURCE;
    }
    
    void TextureImage::loadFromFile()
    {
        file = requestResource<FILETYPE>(FILE_REQUEST, filePath, {Flags::IMAGE | Flags::READ, 4});

        if (file.wait() != ResourceState::YES) {
            registerAlert("Failed to load image from file!", CRITICAL);
            return;
        }
        auto imageFile = dynamic_cast<ImageFile*>(*file);

        imageSize = static_cast<VkDeviceSize>(imageFile->width) *
                    static_cast<VkDeviceSize>(imageFile->height) *
                    4;

        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(imageFile->width, imageFile->height)))) + 1;

        loadImageToMemory(imageSize, imageFile);

        createImage(imageFile->width, imageFile->height, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
        copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(imageFile->width), static_cast<uint32_t>(imageFile->height));

        generateMipmaps(VK_FORMAT_R8G8B8A8_SRGB, imageFile->width, imageFile->height, mipLevels);

        vkDestroyBuffer(*device, stagingBuffer, nullptr);
        vkFreeMemory(*device, stagingBufferMemory, nullptr);
        stagingBuffer = VK_NULL_HANDLE, stagingBufferMemory = VK_NULL_HANDLE;

        createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
        createSampler();

        imageFile->close();
    }

    void TextureImage::loadImageToMemory(VkDeviceSize imageSize, ImageFile* file)
    {
        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        if (device.wait() != ResourceState::YES) {
			registerAlert("Device died before it was ready to be used.", FATAL);
            return;
		}

        void* data;
        vkMapMemory(*device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, file->pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(*device, stagingBufferMemory);
    }

    void TextureImage::createSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        if (physicalDevice.wait() != ResourceState::YES) {
            registerAlert("Physical Device died before it was ready to be used.", FATAL);
            return;
        }
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(*physicalDevice, &properties);

        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f; // "Level of detail"
        samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(*device, &samplerInfo, nullptr, &imageSampler) != VK_SUCCESS) {
            registerAlert("Failed to create texture sampler!", FATAL);
            return;
        }
    }
}