#pragma once
#include "Buffer.h"

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

		void createImage(uint32_t width, uint32_t height, VkFormat format, 
        VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		void setImage(VkImage& image, bool isOwning);
		void createImageView(VkFormat imageViewFormat, VkImageAspectFlags aspectFlags);

		void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		VkImage& getImage() { return image; }
		VkImageView& getImageView() { return imageView; }

		virtual GET_RESOURCE { return {}; };
        virtual GET_RESOURCE_FROM_STRING { return INVALID_RESOURCE; };

		virtual ASSET_NAME("ImageBuffer")
	protected:
		void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);

		VkDeviceSize imageSize = 0;

		VkImage image = VK_NULL_HANDLE;
		VkDeviceMemory imageMemory = VK_NULL_HANDLE;

		VkImageView imageView = VK_NULL_HANDLE;

		bool isOwning = true;
	};
}