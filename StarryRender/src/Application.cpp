#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION
#define STARRY_INITIALIZE_SUCCESS \
	std::cout << "----------------------------------------\n"; \
	std::cout << "Starry Render initialized successfully!\n"; \
	std::cout << "----------------------------------------\n" << std::endl

#define STARRY_EXIT_SUCCESS \
	std::cout << "----------------------------------------\n"; \
	std::cout << "Starry Render exited successfully!\n"; \
	std::cout << "----------------------------------------\n" << std::endl

#else
#define INITIALIZE_SUCCESS
#define EXIT_SUCCESS

#endif

#include "Application.h"

#define CHECK_ERROR(obj) \
	if (obj->getError()) { \
		std::cerr << "Program ended prematurly due to an error." << std::endl; \
	}

namespace StarryRender {
	void Application::init() {
		window = new Window{}; CHECK_ERROR(window);
		renderer = new RenderDevice{window}; CHECK_ERROR(renderer);
		
		STARRY_INITIALIZE_SUCCESS;
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

		STARRY_EXIT_SUCCESS;
	}
}