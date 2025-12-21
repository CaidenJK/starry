#include "Scene.h"

#include "Timer.h"

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; }

#define ERROR_HANDLER AssetManager::get().lock()

namespace Starry
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
		camera.reset();
	}

	void Scene::makeRenderContext()
	{
		if (renderRunning.load()) {
			registerAlert("Cannot create Render Context while rendering!", CRITICAL);
			return;
		}
		renderer = std::make_shared<RenderContext>();
		renderer->loadShaders(shaderPaths); EXTERN_ERROR(renderer);
		renderer->Init(); EXTERN_ERROR(renderer);
	}

	void Scene::makeRenderContext(std::shared_ptr<Window>& window)
	{
		if (renderRunning.load()) {
			registerAlert("Cannot create Render Context while rendering!", CRITICAL);
			return;
		}
		renderer = std::make_shared<RenderContext>();
		renderer->loadShaders(shaderPaths); EXTERN_ERROR(renderer);
		renderer->Init(window); EXTERN_ERROR(renderer);
	}

	void Scene::addRenderContext(std::shared_ptr<RenderContext>& renderContext) 
	{
		if (renderRunning.load()) {
			registerAlert("Cannot change Render Context while rendering!", CRITICAL);
			return;
		}
		renderer = renderContext;
	}

	void Scene::pushPrefab(std::shared_ptr<MeshObject>& prefab)
	{
		if (prefab->isEmptyMesh() == true) {
			registerAlert("Prefab pointer is null!", CRITICAL);
			return;
		}
		prefabs = prefab;
	}

	void Scene::setShaderPaths(const std::array<std::string, 2>& paths) 
	{
		shaderPaths = paths;
		if (renderer != nullptr) {
			renderer->loadShaders(shaderPaths);
		}
	}

	void Scene::addCamera(std::shared_ptr<CameraObject>& cameraRef)
	{
		camera = cameraRef;
	}

	void Scene::disbatchRenderer() 
	{
		if (renderer == nullptr) {
			registerAlert("Render Context not attatched to scene!", FATAL);
			return;
		}
		if (renderer->getAlertSeverity() == FATAL) {
			registerAlert("Render Context experienced a fatal error, could not disbatch renderer", FATAL);
			return;
		}
		if (prefabs->isEmptyMesh() == true) {
			registerAlert("No prefabs in scene to render!", FATAL);
			return;
		}

		prefabs->registerMeshBuffer(renderer);

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
			prefabs->rotateMesh(frameTimer.getDeltaTimeSeconds() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 1.0f));

			UniformBufferData mvpBuffer{prefabs->getModelMatrix(),  camera->getViewMatrix(), camera->getProjectionMatrix()};
			renderer->updateUniformBuffer(mvpBuffer);

			// Error checks
			if (renderer->getRenderErrorState()) {
				registerAlert("Fatal rendering error occurred!", FATAL);
				renderRunning.store(false);
				continue;
			}
			if (ERROR_HANDLER->isFatal()) {
				renderRunning.store(false);
				continue;
			}

			renderer->Draw();
			camera->setExtent(renderer->getExtent());
			
			// Error checks
			if (renderer->getRenderErrorState()) {
				registerAlert("Fatal rendering error occurred!", FATAL);
				renderRunning.store(false);
				continue;
			}
			if (ERROR_HANDLER->isFatal()) {
				renderRunning.store(false);
				continue;
			}
			// Simple frame cap
			std::this_thread::sleep_for(std::chrono::milliseconds(RENDER_LOOP_DELAY_MS));
		}
	}
}