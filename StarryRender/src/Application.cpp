#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#define SUCCESS_VALIDATION
#include "Application.h"

#define CHECK_ERROR(obj) \
	if (obj->getError()) { \
		std::cerr << "Program ended prematurly due to an error." << std::endl; \
	}

namespace StarryRender {
	void Application::initVulkan() {
		window = new Window{};
		CHECK_ERROR(window);
		renderer = new Renderer{};
		CHECK_ERROR(renderer);

#ifdef SUCCESS_VALIDATION
		std::cout << "Starry Render initialized successfully!\n" << std::endl;
#endif
	}
	void Application::renderLoop() {
		while (!window->shouldClose()) {
			window->pollEvents();
		}
	}

	// Destroy renderer then window last
	void Application::cleanup() {
		delete renderer;
		delete window;

#ifdef SUCCESS_VALIDATION
		std::cout << "Starry Render exited successfully!\n" << std::endl;
#endif
	}
}