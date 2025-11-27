#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define DEFAULT_HEIGHT 600
#define DEFAULT_WIDTH 800
#define DEFAULT_TITLE "Starry"

namespace StarryRender
{
	class Window {
	public:
		Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT, const char* title = DEFAULT_TITLE);
		~Window();

		bool shouldClose() const;
		void pollEvents() const;
		GLFWwindow* getGLFWwindow() const;

		void createVulkanSurface(VkInstance& instance, VkSurfaceKHR& surface);
		void getFramebufferSize(int& width, int& height);

		bool getError() { return error; }

		bool wasFramebufferResized() { return framebufferResized; }
		void resetFramebufferResizedFlag() { framebufferResized = false; }

		void windowMinimizedBlock();

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	private:
		void initWindow();

		int width;
		int height;
		const char* title;
		GLFWwindow* window;

		bool framebufferResized = false;

		bool error = false;
	};
}