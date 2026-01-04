#pragma once

#include <StarryManager.h>

#include <vector>
#include <memory>
#include <thread>

#include "SceneObject.h"
#include "Renderer.h"

#define DEFAULT_SCENE_NAME "New Scene"

namespace Starry
{
	class STARRY_API Scene : public StarryAsset {
	public:
		Scene(const std::string name);
		Scene(const std::string name, std::shared_ptr<Window>& window);
		~Scene();

		Scene operator=(const Scene&) = delete;
		Scene(const Scene&) = delete;

		void attatchRenderer(std::shared_ptr<Renderer>& renderContext);

		void pushObject(std::shared_ptr<SceneObject>& obj);
		void pushObjects(std::vector<std::shared_ptr<SceneObject>>& objs);

		void loadObjects(Renderer* renderer);
		void updateObjects(Renderer* renderer);

		ASSET_NAME("Scene: " + sceneName)
	private:
		std::string sceneName = DEFAULT_SCENE_NAME;

		std::map<std::string, std::shared_ptr<SceneObject>> sceneObjects;
		// TODO: something wrong with stack allocated StarryAssets
	};
}
// TODO: seperate out renderer