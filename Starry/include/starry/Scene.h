#pragma once

#include <StarryRender.h>
#include <StarryManager.h>

#include <vector>
#include <memory>
#include <thread>

#include "SceneObject.h"

#define DEFAULT_SCENE_NAME "New Scene"

namespace Starry
{
	class Scene : public StarryAsset {
	public:
		Scene(const std::string name);
		Scene(const std::string name, std::shared_ptr<Window>& window);
		~Scene();

		Scene operator=(const Scene&) = delete;
		Scene(const Scene&) = delete;

		void makeRenderContext();
		void makeRenderContext(std::shared_ptr<Window>& window);
		void addRenderContext(std::shared_ptr<RenderContext>& renderContext);

		void setShaderPaths(const std::array<std::string, 2>& paths);

		void pushObject(std::shared_ptr<SceneObject>& obj);
		void pushObjects(std::vector<std::shared_ptr<SceneObject>>& objs);

		void disbatchRenderer();
		void joinRenderer();

		std::atomic<bool>& isRenderRunning() { return renderRunning; }
		const std::shared_ptr<RenderContext>& getRenderContext() { return renderer; }

		ASSET_NAME("Scene: " + sceneName)
	private:
		void renderLoop();

		inline static constexpr int RENDER_LOOP_DELAY_MS = 2;

		std::string sceneName = DEFAULT_SCENE_NAME;

		std::array<std::string, 2> shaderPaths = DEFAULT_SHADER_PATHS;
		std::shared_ptr<RenderContext> renderer = nullptr;
		
		std::thread renderThread;
		std::atomic<bool> renderRunning{ false };

		std::map<std::string, std::shared_ptr<SceneObject>> sceneObjects;
		// TODO: something wrong with stack allocated StarryAssets

		glm::mat4 modelViewProjection;
	};
}
// TODO: seperate out renderer