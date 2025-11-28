#include "Window.h"

#include <iostream>

#ifndef NDEBUG
#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION

#define THROW_ERROR(msg) \
	error = true; \
	std::cerr << "Window ERROR: " << msg << std::endl; \
	return

#define THROW_ERROR_RETURN(msg, x) \
	error = true; \
	std::cerr << "Window ERROR: " << msg << std::endl; \
	return x


#define ALERT_MSG(msg) \
	std::cout << msg

#else
#define THROW_ERROR(msg) \
	error = true; \
	return

#define ALERT_MSG(msg)

#define THROW_ERROR_RETURN(msg, x) \
	error = true; \
	return x

#endif

namespace StarryRender {
	Window::Window(int width, int height, const char* title)
		: width(width), height(height), title(title), window(nullptr) {
		initWindow();
	}

	Window::~Window() {
		glfwDestroyWindow(window);

		glfwTerminate();
	}
	void Window::initWindow()
	{
		if (!glfwInit()) {
			THROW_ERROR("GLFW initialization failed!");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (!window)
		{
			glfwTerminate();
			THROW_ERROR("GLFW failed to create window!");
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

	void Window::createVulkanSurface(VkInstance& instance, VkSurfaceKHR& surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			THROW_ERROR("Failed to create window surface!");
		}
	}

	void Window::getFramebufferSize(int& width, int& height) {
		glfwGetFramebufferSize(window, &width, &height);
	}

	bool Window::isWindowMinimized() {
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		if (width == 0 || height == 0) {
			return true;
		}
		else {
			return false;
		}
	}

	void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
}