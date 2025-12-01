#include "Scene.h"

#include "Timer.h"

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; }

#define ERROR_HANDLER ErrorHandler::get().lock()
#define ERROR_HANDLER_CHECK CHECK_ERROR(ERROR_HANDLER)

namespace StarryRender 
{
	Scene::Scene(const std::string name) : sceneName(name) 
	{

	}

	Scene::~Scene() 
	{
		if (renderRunning.load()) {
			joinRenderer();
		}
		prefabs.reset();
		renderer.reset();
	}

	void Scene::createDevice(std::shared_ptr<Window> window) 
	{
		renderer = std::make_unique<RenderDevice>(window); EXTERN_ERROR(renderer);

		if (shaderPaths[0].empty() || shaderPaths[1].empty()) {
			registerAlert("Shader paths not set before creating device!", CRITICAL);
			return;
		}
		renderer->LoadShader(shaderPaths[0], shaderPaths[1]); EXTERN_ERROR(renderer);
		renderer->InitDraw(); EXTERN_ERROR(renderer);
	}

	void Scene::attatchDevice(const std::unique_ptr<RenderDevice>& device) 
	{
		if (device == nullptr) {
			registerAlert("Render device pointer is null!", CRITICAL);
			return;
		}
		renderer = std::move(const_cast<std::unique_ptr<RenderDevice>&>(device));
	}

	void Scene::pushPrefab(const std::shared_ptr<MeshObject>& prefab) 
	{
		if (prefab == nullptr) {
			registerAlert("Prefab pointer is null!", CRITICAL);
			return;
		}
		prefabs = prefab;
	}

	void Scene::setShaderPaths(const std::array<std::string, 2>& paths) 
	{
		shaderPaths = paths;
	}

	void Scene::addCamera(const std::shared_ptr<CameraObject>& cameraRef) 
	{
		if (cameraRef == nullptr) {
			registerAlert("Camera pointer is null!", CRITICAL);
			return;
		}
		camera = cameraRef;
	}

	void Scene::disbatchRenderer() 
	{
		if (renderer == nullptr) {
			registerAlert("Render device not attatched to scene!", FATAL);
			return;
		}
		if (prefabs == nullptr) {
			registerAlert("No prefabs in scene to render!", FATAL);
			return;
		}

		prefabs->attatchBuffer(renderer->getDevice(), renderer->getPhysicalDevice()); EXTERN_ERROR(prefabs);
		renderer->LoadBuffer(prefabs->getRawVertexBuffer()); EXTERN_ERROR(renderer);

		renderRunning.store(true);
		renderThread = std::thread(&Scene::renderLoop, this);
	}

	void Scene::joinRenderer() 
	{
		renderRunning.store(false);
		if (renderThread.joinable()) {
			renderThread.join();
		}
		renderer->WaitIdle();
	}

	void Scene::renderLoop() 
	{
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
}