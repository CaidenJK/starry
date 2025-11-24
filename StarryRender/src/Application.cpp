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
#define STARRY_INITIALIZE_SUCCESS
#define STARRY_EXIT_SUCCESS

#endif

#include "Application.h"

#define CHECK_ERROR(obj) \
	error = obj->getError(); \
	if (error) { \
		std::cerr << "\n----------> Program ended prematurly due to an error.\n" << std::endl; \
		return; \
	}

#define ERROR_VOLATILE(x) x; if (error) { return; }

namespace StarryRender {
	void Application::init() {
		window = std::make_shared<Window>(); CHECK_ERROR(window);
		renderer = std::make_shared<RenderDevice>(window); CHECK_ERROR(renderer);

		pipeline = std::make_shared<RenderPipeline>("../../../StarryRender/shaders/vert.spv", "../../../StarryRender/shaders/frag.spv"); CHECK_ERROR(pipeline);
		renderer->setPipeline(pipeline); CHECK_ERROR(renderer);
		// Instead of Check error, just have if(error) {return} at the header of each member function. 
		// That way it'll just run through every function and declare an error at the end cleanly.
		
		STARRY_INITIALIZE_SUCCESS;
	}
	void Application::renderLoop() {
		while (!window->shouldClose()) {
			window->pollEvents();
		}
	}

	// Destroy renderer then window last
	void Application::cleanup() {
		renderer.reset();
		window.reset();

		STARRY_EXIT_SUCCESS;
	}

	void Application::run() {
		printVersion();
		ERROR_VOLATILE(init());
		renderLoop();
		cleanup();
	}
}