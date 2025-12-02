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

#define EXTERN_ERROR(x) \
	if (x->isFatal()) { \
		return; \
	}

#define ERROR_HANDLER ErrorHandler::get().lock()
#define ERROR_HANDLER_CHECK EXTERN_ERROR(ERROR_HANDLER)

namespace StarryRender 
{
	void Application::init() 
	{
		window = std::make_shared<Window>("Starry Dev"); ERROR_HANDLER_CHECK;
		scene = std::make_shared<Scene>("Main Scene"); ERROR_HANDLER_CHECK;

		scene->setShaderPaths({ "../../../StarryRender/shaders/vert.spv", "../../../StarryRender/shaders/frag.spv" }); ERROR_HANDLER_CHECK;
		scene->createDevice(window); ERROR_HANDLER_CHECK;

		CameraObject camera;
		camera.setFOV(60.0f);

		scene->pushPrefab(MeshObject::primitiveCube(1)); ERROR_HANDLER_CHECK;
		scene->addCamera(camera); ERROR_HANDLER_CHECK;

		ERROR_HANDLER_CHECK;
		STARRY_INITIALIZE_SUCCESS;
	}
	void Application::mainLoop() 
	{
		scene->disbatchRenderer(); ERROR_HANDLER_CHECK;

		while (!window->shouldClose() && scene->isRenderRunning().load()) {
			window->pollEvents();
		}

		scene->joinRenderer();
	}

	// Destroy renderer then window last
	void Application::cleanup() 
	{
		scene.reset();
		window.reset();

		if (ERROR_HANDLER->isFatal()) {
			std::cerr << "\n----------> Program ended prematurly due to an error.\n" << std::endl;
			return;
		}
		STARRY_EXIT_SUCCESS;
	}

	void Application::run() 
	{
		printVersion();
		init();
		mainLoop();
		cleanup();
	}
}