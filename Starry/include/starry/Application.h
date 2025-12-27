#pragma once

#include <StarryRender.h>
#include <StarryAsset.h>

#include <memory>

#include "Scene.h"

#include "CoreDLL.h"

#define STARRY_VERSION "0.0.4 dev"
 
namespace Starry
{
    class STARRY_API Application : public StarryAsset {
    public:
        Application() {};
        ~Application() {};

		Application operator=(const Application&) = delete;
		Application(const Application&) = delete;

        void run();

        bool hasFatalError();

        static void enableFileLogging(bool value) { AssetManager::get().lock()->setFileLogging(value); }

		const std::string getAssetName() override { return "Application"; }

    private:
        void printVersion() {
            registerAlert("Hello! Starry Version: " STARRY_VERSION "\n", BANNER);
        }
        void init();
        void mainLoop();
        void cleanup();

        std::shared_ptr<Scene> m_scene = nullptr;
    };
}