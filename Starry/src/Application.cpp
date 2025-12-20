#include "Application.h"

#include "MeshObject.h"

#include <iostream>

#define STARRY_INITIALIZE_SUCCESS \
	"----------------------------------------\n" \
	"Starry initialized successfully!\n" \
	"----------------------------------------\n"

#define STARRY_EXIT_SUCCESS \
	"----------------------------------------\n" \
	"Starry exited successfully!\n" \
	"----------------------------------------\n"

#define EXTERN_ERROR(x) \
	if (x->isFatal()) { \
		return; \
	}
  
#define ERROR_HANDLER Logger::get().lock()
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

		std::shared_ptr<CameraObject> camera = std::make_shared<CameraObject>();
		camera->setFOV(60.0f);

		std::shared_ptr<MeshObject> cube = std::make_shared<MeshObject>("Unit Cube");
		MeshObject::primitiveCube(*cube, 1.0f);

		m_scene->pushPrefab(cube); ERROR_HANDLER_CHECK;
		m_scene->addCamera(camera); ERROR_HANDLER_CHECK;

		ERROR_HANDLER_CHECK;
		registerAlert(STARRY_INITIALIZE_SUCCESS, BANNER);
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
			registerAlert("\n----------> Program ended prematurly due to an error.\n", BANNER);
			return;
		}
		Logger::get().lock()->dumpRegisteredAssets(true);
		registerAlert(STARRY_EXIT_SUCCESS, BANNER);
	}

	void Application::run() 
	{
		printVersion();
		init();
		if (!ERROR_HANDLER->isFatal()) {
			mainLoop();
		}
		cleanup();
	}

	bool Application::hasFatalError() {
		auto errorHandler = Logger::get().lock();
		return errorHandler->isFatal();
	}
}