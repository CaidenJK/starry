#include "Application.h"

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

#define CHECK_ERROR(x) \
	if (x->isFatal()) { \
		return; \
	}

#define ERROR_HANDLER ErrorHandler::get().lock()
#define ERROR_HANDLER_CHECK CHECK_ERROR(ERROR_HANDLER)

#include "Timer.h"

namespace StarryRender {
	void Application::init() {
		window = std::make_shared<Window>(); ERROR_HANDLER_CHECK;
		renderer = std::make_shared<RenderDevice>(window); ERROR_HANDLER_CHECK;

		renderer->LoadShader("../../../StarryRender/shaders/vert.spv", "../../../StarryRender/shaders/frag.spv"); ERROR_HANDLER_CHECK;

		std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f}, RED_COLOR},
			{{-0.5f, 0.8f}, BLUE_COLOR},
			{{0.5f, 0.5f}, GREEN_COLOR},
			{{0.5f, -0.4f}, YELLOW_COLOR}
		};
		const std::vector<uint32_t> indices = {
			0, 1, 2, 0, 2, 3
		};

		auto vertexBuffer = std::make_shared<VertexBuffer>(renderer->getDevice()); ERROR_HANDLER_CHECK;
		vertexBuffer->loadData(renderer->getPhysicalDevice(), vertices, indices); ERROR_HANDLER_CHECK;

		renderer->InitDraw(); ERROR_HANDLER_CHECK;
		renderer->LoadBuffer(vertexBuffer); ERROR_HANDLER_CHECK;


		renderRunning.store(true);
		
		ERROR_HANDLER_CHECK;
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
		Timer frameTimer;
		frameTimer.setLogging();
		while (renderRunning.load()) {
			frameTimer.time();
			renderer->Draw();
			if (ERROR_HANDLER->isFatal()) {
				renderRunning.store(false);
				continue;
			}

			// Simple frame cap
			std::this_thread::sleep_for(std::chrono::milliseconds(RENDER_LOOP_DELAY_MS));
		}
	}

	// Destroy renderer then window last
	void Application::cleanup() {
		renderer.reset();
		window.reset();

		if (ERROR_HANDLER->isFatal()) {
			std::cerr << "\n----------> Program ended prematurly due to an error.\n" << std::endl;
			return;
		}
		STARRY_EXIT_SUCCESS;
	}

	void Application::run() {
		printVersion();
		init();
		mainLoop();
		cleanup();
	}
}