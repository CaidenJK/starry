#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace StarryRender
{
	class ImageTexture
	{
	public:
		ImageTexture();
		~ImageTexture();
	private:
		int texWidth;
		int texHeight;
		int texChannels;


	};
}