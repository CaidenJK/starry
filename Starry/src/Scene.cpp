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
		for (auto& obj : sceneObjects) {
			obj.second->Destroy();
			obj.second.reset();
		}
		renderer.reset();
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

	void Scene::pushObject(std::shared_ptr<SceneObject>& obj)
	{
		obj->Init(); EXTERN_ERROR(obj);
		sceneObjects.emplace(obj->getName(), obj);
	}

	void Scene::pushObjects(std::vector<std::shared_ptr<SceneObject>>& objs)
	{
		for (auto& obj : objs) {
			obj->Init();
			sceneObjects.emplace(obj->getName(), obj);
		}
	}

	void Scene::setShaderPaths(const std::array<std::string, 2>& paths) 
	{
		shaderPaths = paths;
		if (renderer != nullptr) {
			renderer->loadShaders(shaderPaths);
		}
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
		if (sceneObjects.size() == 0) {
			registerAlert("No objects in scene to render!", FATAL);
			return;
		}

		for (auto& obj : sceneObjects) {
			obj.second->Register(renderer); EXTERN_ERROR(obj.second);
		}

		renderer->LoadBuffers(); EXTERN_ERROR(renderer);

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
			for (auto& obj : sceneObjects) {
				obj.second->Update(renderer);
			}

			sceneObjects.find("Mesh, Radio")->second->rotate(frameTimer.getDeltaTimeSeconds() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));


			UniformBufferData mvpBuffer{ sceneObjects.find("Mesh, Radio")->second->getBufferData().model,  
				sceneObjects.find("Camera, Default")->second->getBufferData().view, 
				sceneObjects.find("Camera, Default")->second->getBufferData().proj };
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
			// Simple frame cap - Off for benchmarking
			//std::this_thread::sleep_for(std::chrono::milliseconds(RENDER_LOOP_DELAY_MS));
		}
	}
}