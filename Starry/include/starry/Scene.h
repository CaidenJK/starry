#pragma once

#include "StarryRender.h"

#include <vector>
#include <memory>
#include <thread>

#include "MeshObject.h"
#include "CameraObject.h"

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

		void makeRenderContext(std::shared_ptr<Window>& window);
		void makeRenderContext();

		void addRenderContext(std::shared_ptr<RenderContext>& renderContext);

		void setShaderPaths(const std::array<std::string, 2>& paths);

		void pushPrefab(const MeshObject& prefab);
		void addCamera(const CameraObject& cameraRef);

		MeshObject& getPrefab() { return prefabs; }

		void disbatchRenderer();
		void joinRenderer();

		std::atomic<bool>& isRenderRunning() { return renderRunning; }
		const std::shared_ptr<RenderContext>& getRenderContext() { return renderer; }

		const std::string getAssetName() override { return "Scene: " + sceneName; }
	private:
		//void mainLoop();
		void renderLoop();

		static const int RENDER_LOOP_DELAY_MS = 2;

		std::string sceneName = DEFAULT_SCENE_NAME;

		std::array<std::string, 2> shaderPaths = DEFAULT_SHADER_PATHS;
		std::shared_ptr<RenderContext> renderer = nullptr;
		
		std::thread renderThread;
		std::atomic<bool> renderRunning{ false };

		// One for now
		MeshObject prefabs;
		CameraObject camera;

		glm::mat4 modelViewProjection;
	};

	// soon prefab base class
}