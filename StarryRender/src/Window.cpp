#include "Window.h"

#include <iostream>

#ifndef NDEBUG
#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION

#define THROW_ERROR(msg) \
	error = true; \
	std::cerr << "Device ERROR: " << msg << std::endl; \
	return;

#define ALERT_MSG(msg) \
	std::cout << msg

#else
#define THROW_ERROR(msg) \
	error = true; \
	return;

#define ALERT_MSG(msg)

#endif

namespace StarryRender {
	Window::Window(int width, int height, const char* title)
		: width(width), height(height), title(title), window(nullptr)
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
			THROW_ERROR("GLFW initialization failed!");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (!window)
		{
			glfwTerminate();
			THROW_ERROR("GLFW failed to create window!");
		}

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

	void Window::createSurface(VkInstance& instance) {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			THROW_ERROR("Failed to create window surface!");
		}
	}

	void Window::destroySurface(VkInstance& instance) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}

	bool Window::queryDeviceSupportKHR(VkPhysicalDevice& device, int number) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, number, surface, &presentSupport);
		return presentSupport;
	}
}