#include <vulkan/vulkan.h>

#include <iostream>

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

#define CHECK_ERROR(x) \
	if (x->hasError()) { \
		std::cerr << "\n----------> Program ended prematurly due to an error.\n" << std::endl; \
		return; \
	}

#define ERROR_HANDLER ErrorHandler::get().lock()
#define ERROR_HANDLER_CHECK CHECK_ERROR(ERROR_HANDLER)

namespace StarryRender {
	void Application::init() {
		window = std::make_shared<Window>(); ERROR_HANDLER_CHECK;
		renderer = std::make_shared<RenderDevice>(window); ERROR_HANDLER_CHECK;

		renderer->LoadShader("../../../StarryRender/shaders/vert.spv", "../../../StarryRender/shaders/frag.spv"); ERROR_HANDLER_CHECK;
		renderer->InitDraw(); ERROR_HANDLER_CHECK;

		renderRunning.store(true);
		
		STARRY_INITIALIZE_SUCCESS;
	}
	void Application::mainLoop() {
		renderThread = std::thread(&Application::renderLoop, this);

		while (!window->shouldClose() && renderRunning.load()) {
			window->pollEvents();
		}

		renderRunning.store(false);
		if (renderThread.joinable()) renderThread.join();

		renderer->WaitIdle();
	}

	void Application::renderLoop() {

		while (renderRunning.load()) {
			renderer->Draw();
			if (ERROR_HANDLER->hasError()) {
				renderRunning.store(false);
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(RENDER_LOOP_DELAY_MS));
		}
	}

	// Destroy renderer then window last
	void Application::cleanup() {
		renderer.reset();
		window.reset();

		ERROR_HANDLER_CHECK;
		STARRY_EXIT_SUCCESS;
	}

	void Application::run() {
		printVersion();
		init();
		mainLoop();
		cleanup();
	}
}