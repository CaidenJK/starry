#include "Window.h"

#include <iostream>

#ifndef NDEBUG
#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION

#define ALERT_MSG(msg) \
	std::cout << msg

#else

#define ALERT_MSG(msg)

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
			registerError("GLFW initialization failed!");
			return;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (!window) {
			glfwTerminate();
			registerError("GLFW failed to create window!");
			return;
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	}
	bool Window::shouldClose() const {
		return glfwWindowShouldClose(window);
	}

	void Window::pollEvents() const {
		glfwPollEvents();
	}

	GLFWwindow* Window::getGLFWwindow() const {
		return window;
	}

	void Window::createVulkanSurface(VkInstance& instance, VkSurfaceKHR& surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			registerError("Failed to create window surface!");
			return;
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