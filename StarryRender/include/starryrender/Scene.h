#pragma once

#include <vector>
#include <memory>
#include <thread>

#include "RenderDevice.h"
#include "MeshObject.h"
#include "CameraObject.h"
#include "UniformBuffer.h"

#define DEFAULT_SCENE_NAME "New Scene"
#define DEFAULT_SHADER_PATHS { "../../../StarryRender/shaders/vert.spv", "../../../StarryRender/shaders/frag.spv" }

namespace StarryRender 
{
	class Scene : public RenderAsset {
	public:
		Scene(const std::string name);
		~Scene();

		Scene operator=(const Scene&) = delete;
		Scene(const Scene&) = delete;

		void setShaderPaths(const std::array<std::string, 2>& paths);

		void createDevice(std::shared_ptr<Window> window);
		void attatchDevice(const std::unique_ptr<RenderDevice>& device);

		void readObjectsFromFile(const std::string& filePath) {}
		void pushPrefab(const MeshObject& prefab);
		void addCamera(const CameraObject& cameraRef);

		MeshObject& getPrefab() { return prefabs; }

		void disbatchRenderer();
		void joinRenderer();

		std::atomic<bool>& isRenderRunning() { return renderRunning; }

		const std::string getAssetName() override { return "Scene: " + sceneName; }
	private:
		//void mainLoop();
		void renderLoop();

		static const int RENDER_LOOP_DELAY_MS = 2;

		std::string sceneName = DEFAULT_SCENE_NAME;

		std::array<std::string, 2> shaderPaths = DEFAULT_SHADER_PATHS;
		std::unique_ptr<RenderDevice> renderer = nullptr;
		
		std::thread renderThread;
		std::atomic<bool> renderRunning{ false };

		// One for now
		MeshObject prefabs;
		CameraObject camera;

		std::shared_ptr<UniformBuffer> uniformBuffer = nullptr;

		glm::mat4 modelViewProjection;
	};

	// soon prefab base class
}