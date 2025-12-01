#include "Application.h"

#include <iostream>

#include "MeshObject.h"

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
		scene = std::make_shared<Scene>("Main Scene"); ERROR_HANDLER_CHECK;

		scene->setShaderPaths({ "../../../StarryRender/shaders/vert.spv", "../../../StarryRender/shaders/frag.spv" }); ERROR_HANDLER_CHECK;
		scene->createDevice(window); ERROR_HANDLER_CHECK;

		std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f}, RED_COLOR},
			{{-0.5f, 0.8f}, BLUE_COLOR},
			{{0.5f, 0.5f}, GREEN_COLOR},
			{{0.5f, -0.4f}, YELLOW_COLOR}
		};
		const std::vector<uint32_t> indices = {
			0, 1, 2, 0, 2, 3
		};

		std::shared_ptr<MeshObject> meshObject = std::make_shared<MeshObject>("Triangle Mesh"); ERROR_HANDLER_CHECK;
		meshObject->addVertexData(vertices, indices); ERROR_HANDLER_CHECK;

		scene->pushPrefab(meshObject); ERROR_HANDLER_CHECK;

		ERROR_HANDLER_CHECK;
		STARRY_INITIALIZE_SUCCESS;
	}
	void Application::mainLoop() {
		scene->disbatchRenderer(); ERROR_HANDLER_CHECK;

		while (!window->shouldClose() && scene->isRenderRunning().load()) {
			window->pollEvents();
		}

		scene->joinRenderer();
	}

	// Destroy renderer then window last
	void Application::cleanup() {
		scene.reset();
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