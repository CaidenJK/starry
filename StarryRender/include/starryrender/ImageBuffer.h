#pragma once
#include "Buffer.h"

#include "../../external/STB_Image/stb_image.h" // FIX INCLUDE

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

		void loadImageFromFile(std::string filePath);

	private:
		void loadFromFile(const char* filePath);
		void loadImageToMemory(VkDeviceSize imageSize);
		void createImageObject(uint32_t width, uint32_t height, VkFormat format, 
        VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
        VkImage& image, VkDeviceMemory& imageMemory);

		void transitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);

		int texWidth = 0;
		int texHeight = 0;
		int texChannels = 0;

		stbi_uc* pixels = nullptr;
		VkDeviceSize imageSize = 0;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;

		VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
	};
}