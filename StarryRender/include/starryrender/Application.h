#pragma once

#include "Asset.h"
#include "Window.h"
#include "RenderDevice.h"

#include <iostream>
#include <memory>
#include <thread>
#include <atomic>

#define STARRY_APP_VERSION "0.0.1"
 
namespace StarryRender {
    class Application {
    public:
        Application() {};
        ~Application() {};

		Application operator=(const Application&) = delete;
		Application(const Application&) = delete;

        void run();

    private:
        void printVersion() {
            std::cout << "Hello! Starry Application Version: " << STARRY_APP_VERSION << "\n" << std::endl;
        }
        void init();
        void mainLoop();
        void cleanup();

        static const int RENDER_LOOP_DELAY_MS = 6;

        std::thread renderThread;
        std::atomic<bool> renderRunning{ false };

        void renderLoop();

        std::shared_ptr<Window> window = nullptr;
        std::shared_ptr<RenderDevice> renderer = nullptr;
		std::shared_ptr<RenderPipeline> pipeline = nullptr;
    };
}