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
  
#define ERROR_HANDLER Starry::AssetManager::get().lock()
#define ERROR_HANDLER_CHECK EXTERN_ERROR(ERROR_HANDLER)

namespace Editor
{
	void Application::setConfig(const ApplicationConfig& config) 
	{
		Starry::AssetManager::InitManager(config.packageName);

		auto manager = Starry::AssetManager::get().lock();
		if (!manager) return;
		manager->setFileLogging(config.enableFileLogging);
		manager->setExitRights(config.managerExitRights);
	}

	void Application::init() 
	{
		m_scene = std::make_shared<Starry::Scene>("Main Scene"); ERROR_HANDLER_CHECK;
		m_window = std::make_shared<Starry::Window>("Main Window"); ERROR_HANDLER_CHECK;
#ifdef SHADERS_PATH
		Starry::RenderConfig config(
			SHADERS_PATH "vert.spv",
			SHADERS_PATH "frag.spv",
			Starry::RenderConfig::MSAAOptions::MSAA_8X,
			{0.05, 0.05, 0.05}
		);
#else
#error "SHADERS_PATH not defined!"
#endif
		m_renderer = std::make_shared<Starry::Renderer>(m_window, config); ERROR_HANDLER_CHECK;
		m_renderer->setScene(m_scene);

		m_metricDisplay = std::make_shared<FrameMetricDisplay>();
		m_metricDisplay->Init(m_renderer->getUUID());
		auto ptr = static_pointer_cast<Starry::UIElement>(m_metricDisplay);
		m_renderer->loadUIElement(ptr, 1);

		std::shared_ptr<Starry::CameraObject> camera = std::make_shared<Starry::CameraObject>();
		camera->setFOV(60.0f);

		std::shared_ptr<Starry::MeshObject> radio = std::make_shared<Starry::MeshObject>("Radio");
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

		std::vector<std::shared_ptr<Starry::SceneObject>> objects = { static_cast<std::shared_ptr<Starry::SceneObject>>(radio),  static_cast<std::shared_ptr<Starry::SceneObject>>(camera) };

		m_scene->pushObjects(objects); ERROR_HANDLER_CHECK;

		ERROR_HANDLER_CHECK;
		Alert(STARRY_INITIALIZE_SUCCESS, BANNER);
	}
	void Application::mainLoop() 
	{
		m_renderer->disbatchRenderer(); ERROR_HANDLER_CHECK;

		while (!m_window->shouldClose() && m_renderer->isRenderRunning().load()) {
			m_window->pollEvents();
			m_renderer->UIPollEvents();
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
			Alert("\n----------> Program ended prematurly due to an error.\n", BANNER);
			return;
		}
		Starry::AssetManager::get().lock()->dumpRegisteredAssets(true);
		Alert(STARRY_EXIT_SUCCESS, BANNER);
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
		auto errorHandler = Starry::AssetManager::get().lock();
		return errorHandler->isFatal();
	}
}