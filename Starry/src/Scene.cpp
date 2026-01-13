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
			obj->Init(); EXTERN_ERROR(obj);
			sceneObjects.emplace(obj->getName(), obj);
		}
	}

	void Scene::loadObjects(Renderer* renderer)
	{
		if (sceneObjects.size() == 0) {
			Alert("No objects in scene to render!", FATAL);
			return;
		}
		if (renderer == nullptr) {
			Alert("Renderer is null!", FATAL);
			return;
		}
		for (auto& obj : sceneObjects) {
			obj.second->Register(renderer); EXTERN_ERROR(obj.second);
		}
	}

	void Scene::updateObjects(Renderer* renderer)
	{
		if (renderer == nullptr) {
			Alert("Renderer is null!", FATAL);
			return;
		}
		glm::mat4 view(1);
		glm::mat4 proj(1);

		for (auto& obj : sceneObjects) {
			obj.second->Update(renderer); EXTERN_ERROR(obj.second);
			if (obj.second->getType() == SceneObject::Type::CAMERA) {
				view = obj.second->getBufferData().view;
				proj = obj.second->getBufferData().proj;
			}
		}
		for (auto& obj : sceneObjects) {
			if (obj.second->getType() == SceneObject::Type::MESH) {
				obj.second->setView(view);
				obj.second->setProjection(proj);
			}
		}
	}
}