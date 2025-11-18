#include "Window.h"

#include <iostream>

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
			error = true;
#ifdef SUCCESS_VALIDATION
			std::cerr << "GLFW initialization failed!" << std::endl;
#endif
			return;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (!window)
		{
			glfwTerminate();
			error = true;
#ifdef SUCCESS_VALIDATION
			std::cerr << "GLFW failed to create window!" << std::endl;
#endif
			return;
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
}