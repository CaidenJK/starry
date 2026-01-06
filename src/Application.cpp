#include "Application.h"

#include <string>

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
  
#define ERROR_HANDLER AssetManager::get().lock()
#define ERROR_HANDLER_CHECK EXTERN_ERROR(ERROR_HANDLER)

namespace Editor
{
	void Application::setConfig(const ApplicationConfig& config) 
	{
		AssetManager::InitManager(config.packageName);

		auto manager = AssetManager::get().lock();
		if (!manager) return;
		manager->setFileLogging(config.enableFileLogging);
		manager->setExitRights(config.managerExitRights);
	}

	void Application::init() 
	{
		m_scene = std::make_shared<Scene>("Main Scene"); ERROR_HANDLER_CHECK;
		m_window = std::make_shared<Window>("Main Window"); ERROR_HANDLER_CHECK;
#ifdef SHADERS_PATH
		RenderConfig config = constructRenderConfig(
			SHADERS_PATH "vert.spv",
			SHADERS_PATH "frag.spv",
			MSAAOptions::MSAA_8X
		);
#else
#error "SHADERS_PATH not defined!"
#endif
		m_renderer = std::make_shared<Renderer>(m_window, config); ERROR_HANDLER_CHECK;
		m_renderer->setScene(m_scene);

		std::shared_ptr<CameraObject> camera = std::make_shared<CameraObject>();
		camera->setFOV(60.0f);

		std::shared_ptr<MeshObject> radio = std::make_shared<MeshObject>("Radio");
#ifdef MODEL_PATH
		radio->loadMeshFromFile(MODEL_PATH "radio.obj");
#else
#error "MODEL_PATH not defined!"
#endif
		
#ifdef IMAGE_PATH
		radio->loadTextureFromFile( IMAGE_PATH "radio.png");
#else
#error "IMAGE_PATH not defined!"
#endif

		std::vector<std::shared_ptr<SceneObject>> objects = { static_cast<std::shared_ptr<SceneObject>>(radio),  static_cast<std::shared_ptr<SceneObject>>(camera) };

		m_scene->pushObjects(objects); ERROR_HANDLER_CHECK;

		ERROR_HANDLER_CHECK;
		registerAlert(STARRY_INITIALIZE_SUCCESS, BANNER);
	}
	void Application::mainLoop() 
	{
		m_renderer->disbatchRenderer(); ERROR_HANDLER_CHECK;

		while (!m_window->shouldClose() && m_renderer->isRenderRunning().load()) {
			m_window->pollEvents();

		}
		m_renderer->joinRenderer();
	}

	// Destroy renderer then window last
	void Application::cleanup() 
	{
		m_scene.reset();
		m_renderer.reset();
		m_window.reset();

		if (ERROR_HANDLER->isFatal()) {
			registerAlert("\n----------> Program ended prematurly due to an error.\n", BANNER);
			return;
		}
		AssetManager::get().lock()->dumpRegisteredAssets(true);
		registerAlert(STARRY_EXIT_SUCCESS, BANNER);
	}

	void Application::run() 
	{
		init();
		if (!ERROR_HANDLER->isFatal()) {
			mainLoop();
		}
		cleanup();
	}

	bool Application::hasFatalError() {
		auto errorHandler = AssetManager::get().lock();
		return errorHandler->isFatal();
	}
}