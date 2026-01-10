#include "Renderer.h"

#include "Scene.h"

#define EXTERN_ERROR(x) if(x.getAlertSeverity() == FATAL) { return; }
#define EXTERN_ERROR_PTR(x) if(x->getAlertSeverity() == FATAL) { return; }

namespace Starry
{
	Renderer::Renderer(std::shared_ptr<Window>& windowRef, RenderConfig config)
	{
		renderer.Init(windowRef, config); EXTERN_ERROR(renderer);
		uniformBuffer = std::make_shared<UniformBuffer>();
	}

	Renderer::~Renderer()
	{
		if (renderRunning.load()) {
			joinRenderer();
		}
		activeScene.reset();
	}

	void Renderer::loadCanvas(std::shared_ptr<Canvas> cnvs)
	{
		renderer.Load(cnvs);
	}

	void Renderer::disbatchRenderer()
	{
		if (renderer.getAlertSeverity() == FATAL) {
			Alert("Render Context experienced a fatal error, could not disbatch renderer", FATAL);
			return;
		}

		activeScene->loadObjects(this); EXTERN_ERROR_PTR(activeScene);

		renderer.Load(uniformBuffer);
		renderer.Ready(); EXTERN_ERROR(renderer);

		renderRunning.store(true);
		renderThread = std::thread(&Renderer::renderLoop, this);
	}

	void Renderer::joinRenderer()
	{
		renderRunning.store(false);
		if (renderThread.joinable()) {
			renderThread.join();
		}
		renderer.WaitIdle();
	}

	void Renderer::renderLoop()
	{
		timer.setLogging();
		while (renderRunning.load()) {
			timer.time();

			activeScene->updateObjects(this); EXTERN_ERROR_PTR(activeScene);

			// Error checks
			if (renderer.getErrorState()) {
				Alert("Fatal rendering error occurred!", FATAL);
				renderRunning.store(false);
				continue;
			}
			if (AssetManager::get().lock()->isFatal()) {
				renderRunning.store(false);
				continue;
			}

			renderer.Draw();

			// Error checks
			if (renderer.getErrorState()) {
				Alert("Fatal rendering error occurred!", FATAL);
				renderRunning.store(false);
				continue;
			}
			if (AssetManager::get().lock()->isFatal()) {
				renderRunning.store(false);
				continue;
			}
		}
	}
}