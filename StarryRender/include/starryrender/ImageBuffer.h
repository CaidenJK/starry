#pragma once
#include "Buffer.h"

#include "stb_image.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace StarryRender
{
	class ImageBuffer : public Buffer
	{
	public:
		ImageBuffer();
		~ImageBuffer();

		void loadImageFromFile(const std::string filePath);

		enum SharedResources {
			IMAGE_VIEW = 0,
			SAMPLER = 1
		};

		std::optional<void*> getResource(size_t resourceID) override;
		size_t getResourceIDFromString(std::string resourceName) override;

		const std::string getAssetName() override { return "ImageBuffer"; }
	private:
		void loadFromFile(const char* filePath);
		void loadImageToMemory(VkDeviceSize imageSize);
		void createImageObject(uint32_t width, uint32_t height, VkFormat format, 
        VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
        VkImage& image, VkDeviceMemory& imageMemory);

		void transitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);

		void createTextureImageView();
		void createTextureSampler();

		int texWidth = 0;
		int texHeight = 0;
		int texChannels = 0;

		stbi_uc* pixels = nullptr;
		VkDeviceSize imageSize = 0;

		VkImage textureImage = VK_NULL_HANDLE;
		VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

		VkImageView textureImageView = VK_NULL_HANDLE;
		VkSampler textureSampler = VK_NULL_HANDLE;
	};
}