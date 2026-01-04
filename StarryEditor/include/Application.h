#pragma once

#include <Starry.h>

#include <memory>

#define STARRY_VERSION "0.0.5 dev"
 
namespace Editor
{
    class Application : public StarryAsset {
    public:
        Application() {};
        ~Application() {};

		Application operator=(const Application&) = delete;
		Application(const Application&) = delete;

        void run();

        bool hasFatalError();

        static void enableFileLogging(bool value) { AssetManager::get().lock()->setFileLogging(value); }
        static void setManagerExitRights(bool value) { AssetManager::get().lock()->setExitRights(value); }

		ASSET_NAME("Application")

    private:
        void printVersion() {
            registerAlert("Hello! Starry Version: " STARRY_VERSION "\n", BANNER);
        }
        void init();
        void mainLoop();
        void cleanup();

		std::shared_ptr<Renderer> m_renderer = nullptr;
		std::shared_ptr<Window> m_window = nullptr;
        std::shared_ptr<Scene> m_scene = nullptr;
    };
}