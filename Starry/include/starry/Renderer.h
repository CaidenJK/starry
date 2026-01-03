#pragma once

#include <StarryManager.h>
#include <StarryRender.h>

#include "Timer.h"
#include "Scene.h"

namespace Starry
{
	class Renderer : public StarryAsset {
		public:
			Renderer(std::shared_ptr<Window>& windowRef, RenderConfig config);
			~Renderer();

			void setScene(std::shared_ptr<Scene> scene);

			void setShaderPaths(const std::array<std::string, 2>& paths);

			void disbatchRenderer();
			void joinRenderer();

			std::atomic<bool>& isRenderRunning() { return renderRunning; }

			RenderContext& context() { return renderer; }

			Timer timer = {};

			ASSET_NAME("Renderer")
		private:
			void renderLoop();

			std::array<std::string, 2> shaderPaths = DEFAULT_SHADER_PATHS;
			RenderContext renderer;

			std::thread renderThread;
			std::atomic<bool> renderRunning{ false };

			glm::mat4 modelViewProjection;

			std::shared_ptr<Scene> activeScene = nullptr;
	};
}