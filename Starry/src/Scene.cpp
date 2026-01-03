#include "Scene.h"

#include "Renderer.h"

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; }

namespace Starry
{
	Scene::Scene(const std::string name) : sceneName(name) 
	{
		
	}

	Scene::~Scene() 
	{
		for (auto& obj : sceneObjects) {
			obj.second->Destroy();
			obj.second.reset();
		}
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

	void Scene::loadObjects(Renderer* renderer)
	{
		if (sceneObjects.size() == 0) {
			registerAlert("No objects in scene to render!", FATAL);
			return;
		}
		if (renderer == nullptr) {
			registerAlert("Renderer is null!", FATAL);
			return;
		}
		for (auto& obj : sceneObjects) {
			obj.second->Register(renderer); EXTERN_ERROR(obj.second);
		}
	}

	void Scene::updateObjects(Renderer* renderer)
	{
		if (renderer == nullptr) {
			registerAlert("Renderer is null!", FATAL);
			return;
		}
		for (auto& obj : sceneObjects) {
			obj.second->Update(renderer); EXTERN_ERROR(obj.second);
		}
		sceneObjects.find("Mesh, Radio")->second->rotate(renderer->timer.getDeltaTimeSeconds() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		UniformBufferData mvpBuffer{ sceneObjects.find("Mesh, Radio")->second->getBufferData().model,
				sceneObjects.find("Camera, Default")->second->getBufferData().view,
				sceneObjects.find("Camera, Default")->second->getBufferData().proj };
		renderer->context().updateUniformBuffer(mvpBuffer);
	}
}