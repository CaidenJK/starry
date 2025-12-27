#pragma once
#include "Buffer.h"

#include <stb_image.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace StarryRender
{
	class ImageTexture : public Buffer
	{
	public:
		ImageTexture();
		~ImageTexture();

		void loadImageFromFile(std::string filePath);

	private:
		void loadFromFile(const char* filePath);
		void loadImageToMemory(VkDeviceSize imageSize);
		void createImageObject(uint32_t width, uint32_t height, VkFormat format, 
        VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
        VkImage& image, VkDeviceMemory& imageMemory);

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		int texWidth = 0;
		int texHeight = 0;
		int texChannels = 0;

		stbi_uc* pixels = nullptr;
		VkDeviceSize imageSize = 0;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
	};
}