#include "Application.h"

#include <iostream>

#include "MeshObject.h"

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION

#define STARRY_INITIALIZE_SUCCESS \
	std::cout << "----------------------------------------\n"; \
	std::cout << "Starry initialized successfully!\n"; \
	std::cout << "----------------------------------------\n" << std::endl

#define STARRY_EXIT_SUCCESS \
	std::cout << "----------------------------------------\n"; \
	std::cout << "Starry exited successfully!\n"; \
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

namespace Starry
{
	void Application::init() 
	{
		m_scene = std::make_shared<Scene>("Main Scene"); ERROR_HANDLER_CHECK;
#ifdef SHADERS_PATH
		m_scene->setShaderPaths({ SHADERS_PATH "vert.spv", SHADERS_PATH "frag.spv" }); ERROR_HANDLER_CHECK;
#else
	#error "SHADERS_PATH not defined!"
#endif
		m_scene->makeRenderContext(); ERROR_HANDLER_CHECK;

		CameraObject camera;
		camera.setFOV(60.0f);

		m_scene->pushPrefab(MeshObject::primitiveCube(1)); ERROR_HANDLER_CHECK;
		m_scene->addCamera(camera); ERROR_HANDLER_CHECK;

		ERROR_HANDLER_CHECK;
		STARRY_INITIALIZE_SUCCESS;
	}
	void Application::mainLoop() 
	{
		m_scene->disbatchRenderer(); ERROR_HANDLER_CHECK;

		while (!m_scene->getRenderContext()->windowShouldClose() && m_scene->isRenderRunning().load()) {
			m_scene->getRenderContext()->windowPollEvents();
		}

		m_scene->joinRenderer();
	}

	// Destroy renderer then window last
	void Application::cleanup() 
	{
		m_scene.reset();

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

	bool Application::hasFatalError() {
		auto errorHandler = ErrorHandler::get().lock();
		return errorHandler->isFatal();
	}
}