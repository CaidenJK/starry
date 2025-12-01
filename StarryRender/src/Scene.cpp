#include "Scene.h"

#include "Timer.h"

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; }
#define EXTERN_ERROR_STACK(x) if(x.getAlertSeverity() == FATAL) { return; }

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
		prefabs.~MeshObject();

		uniformBuffer.reset();
		renderer.reset();
	}

	void Scene::createDevice(std::shared_ptr<Window> window) 
	{
		renderer = std::make_unique<RenderDevice>(window); EXTERN_ERROR(renderer);

		if (shaderPaths[0].empty() || shaderPaths[1].empty()) {
			registerAlert("Shader paths not set before creating device!", CRITICAL);
			return;
		}
		uniformBuffer = std::make_shared<UniformBuffer>(renderer->getDevice()); EXTERN_ERROR(uniformBuffer);
		renderer->loadUniformBuffer(uniformBuffer);

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

	void Scene::pushPrefab(const MeshObject& prefab) 
	{
		if (prefab.isEmptyMesh() == true) {
			registerAlert("Prefab pointer is null!", CRITICAL);
			return;
		}
		prefabs = prefab;
		if (uniformBuffer == nullptr) {
			registerAlert("Uniform buffer not initialized before pushing prefab!", CRITICAL);
			return;
		}
		uniformBuffer->setModelMatrix(prefabs.getModelMatrix());
	}

	void Scene::setShaderPaths(const std::array<std::string, 2>& paths) 
	{
		shaderPaths = paths;
	}

	void Scene::addCamera(const CameraObject& cameraRef) 
	{
		camera = cameraRef;
	}

	void Scene::disbatchRenderer() 
	{
		if (renderer == nullptr) {
			registerAlert("Render device not attatched to scene!", FATAL);
			return;
		}
		if (prefabs.isEmptyMesh() == true) {
			registerAlert("No prefabs in scene to render!", FATAL);
			return;
		}

		prefabs.attatchBuffer(renderer->getDevice(), renderer->getPhysicalDevice()); EXTERN_ERROR_STACK(prefabs);
		renderer->LoadBuffer(prefabs.getRawVertexBuffer()); EXTERN_ERROR(renderer);

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
			prefabs.rotateMesh(frameTimer.getDeltaTimeSeconds() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 1.0f));

			uniformBuffer->setMVP(
				prefabs.getModelMatrix(),
				camera.getViewMatrix(),
				camera.getProjectionMatrix()
				);
			EXTERN_ERROR(uniformBuffer);

			renderer->Draw();
			camera.setExtent(renderer->getExtent());
			if (ERROR_HANDLER->isFatal()) {
				renderRunning.store(false);
				continue;
			}
			// Simple frame cap
			std::this_thread::sleep_for(std::chrono::milliseconds(RENDER_LOOP_DELAY_MS));
		}
	}
}