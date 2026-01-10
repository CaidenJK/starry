#pragma once

#include <StarryManager.h>
#include <StarryRender.h>

#include "Timer.h"
#include "Interface.h"

namespace Starry
{
	class Scene;

	using RenderConfig = Render::RenderConfig;
	using Window = Render::Window;

	class Renderer : public Manager::StarryAsset {
		public:
			Renderer(std::shared_ptr<Window>& windowRef, RenderConfig config);
			~Renderer();

			void setScene(std::shared_ptr<Scene>& scene) { activeScene = scene; }
			void loadUIElement(std::shared_ptr<UIElement>& elmt, int drawOrder);

			void setShaderPaths(const std::array<std::string, 2>& paths);

			void disbatchRenderer();
			void joinRenderer();
			
			void UIPollEvents() { interface->PollEvents(); }

			void updateUniform(Render::UniformData data) { Uniform->setData(data); }
			std::atomic<bool>& isRenderRunning() { return renderRunning; }

			Render::RenderContext& context() { return renderer; }
			Timer timer = {};

			void askCallback(std::shared_ptr<Manager::ResourceAsk>& ask) override;

			ASSET_NAME("Renderer")
		private:
			void renderLoop();

			std::array<std::string, 2> shaderPaths = DEFAULT_SHADER_PATHS;
			Render::RenderContext renderer{};

			std::thread renderThread;
			std::atomic<bool> renderRunning{ false };

			glm::mat4 modelViewProjection;

			std::shared_ptr<Scene> activeScene = nullptr;
			std::shared_ptr<Interface> interface = nullptr;

			std::shared_ptr<Render::Uniform> Uniform = nullptr;
	};
}