#include "Window.h"

namespace StarryRender 
{
	Window::Window(const char* title)
		: title(title), window(nullptr) 
	{
		initWindow();
	}

	Window::~Window() 
	{
		glfwDestroyWindow(window);

		glfwTerminate();
	}
	void Window::initWindow()
	{
		if (!glfwInit()) {
			registerAlert("GLFW initialization failed!", FATAL);
			return;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (!window) {
			glfwTerminate();
			registerAlert("GLFW failed to create window!", FATAL);
			return;
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	}
	bool Window::shouldClose() const 
	{
		return glfwWindowShouldClose(window);
	}

	void Window::pollEvents() const 
	{
		glfwPollEvents();
	}

	GLFWwindow* Window::getGLFWwindow() const 
	{
		return window;
	}

	void Window::createVulkanSurface(VkInstance& instance, VkSurfaceKHR& surface) 
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			registerAlert("Failed to create window surface!", FATAL);
			return;
		}
	}

	void Window::getFramebufferSize(int& width, int& height) 
	{
		glfwGetFramebufferSize(window, &width, &height);
	}

	bool Window::isWindowMinimized() 
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		if (width == 0 || height == 0) {
			return true;
		}
		else {
			return false;
		}
	}

	void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) 
	{
		auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->framebufferResized.store(true);
	}
}