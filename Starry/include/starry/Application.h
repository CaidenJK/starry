#pragma once

#include "StarryRender.h"

#include <iostream>
#include <memory>
#include <thread>
#include <atomic>

#include "Asset.h"
#include "Scene.h"

#define STARRY_VERSION "0.0.3 dev"
 
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

		const std::string getAssetName() override { return "Application"; }

    private:
        void printVersion() {
            std::cout << "Hello! Starry Version: " << STARRY_VERSION << "\n" << std::endl;
        }
        void init();
        void mainLoop();
        void cleanup();

        std::shared_ptr<Scene> m_scene = nullptr;
    };
}