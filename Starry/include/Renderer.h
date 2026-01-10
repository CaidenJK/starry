#pragma once

#include <StarryManager.h>
#include <StarryRender.h>

#include "Timer.h"

namespace Starry
{
	class Scene;

	class STARRY_API Renderer : public StarryAsset {
		public:
			Renderer(std::shared_ptr<Window>& windowRef, RenderConfig config);
			~Renderer();

			void setScene(std::shared_ptr<Scene> scene) { activeScene = scene; }
			void loadCanvas(std::shared_ptr<Canvas> cnvs);

			void setShaderPaths(const std::array<std::string, 2>& paths);

			void disbatchRenderer();
			void joinRenderer();

			void updateUniform(UniformBufferData data) { uniformBuffer->setData(data); }
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

			std::shared_ptr<UniformBuffer> uniformBuffer = nullptr;
	};
}