#pragma once
#include <StarryAsset.h>

#include <stb_image.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace StarryRender
{
	class ImageTexture : StarryAsset
	{
	public:
		ImageTexture();
		~ImageTexture();

		void loadImageFromFile(std::string filePath);

	private:
		void loadFromFile(const char* filePath);
		void loadImageToMemory() = delete;

		int texWidth = 0;
		int texHeight = 0;
		int texChannels = 0;

		stbi_uc* pixels = nullptr;
		VkDeviceSize imageSize = 0;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;

		ResourceHandle<VkDevice> device;
	};
}