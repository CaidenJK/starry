#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define DEFAULT_HEIGHT 600
#define DEFAULT_WIDTH 800
#define DEFAULT_TITLE "Starry"

#include "Asset.h"

namespace StarryRender
{
	class Window : public RenderAsset {
	public:
		Window(const char* title = DEFAULT_TITLE);
		~Window();

		Window operator=(const Window&) = delete;
		Window(const Window&) = delete;

		bool shouldClose() const;
		void pollEvents() const;
		GLFWwindow* getGLFWwindow() const;

		void createVulkanSurface(VkInstance& instance, VkSurfaceKHR& surface);
		void getFramebufferSize(int& width, int& height);

		bool wasFramebufferResized() { return framebufferResized; }
		void resetFramebufferResizedFlag() { framebufferResized = false; }

		bool isWindowMinimized();

		const std::string getAssetName() override { return "Window"; }

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	private:
		void initWindow();

		int width = DEFAULT_WIDTH;
		int height = DEFAULT_HEIGHT;
		const char* title;
		GLFWwindow* window;

		bool framebufferResized = false;
	};
}