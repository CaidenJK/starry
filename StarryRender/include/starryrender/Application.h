#pragma once

#include "Window.h"
#include "RenderDevice.h"

#include <iostream>
#include <memory>

#define STARRY_APP_VERSION "0.0.1"
 
namespace StarryRender {
    class Application {
    public:
        Application() {};
        ~Application() {};

		bool getError() { return error; }

        void run();

    private:
        void printVersion() {
            std::cout << "Hello! Starry Application Version: " << STARRY_APP_VERSION << "\n" << std::endl;
        }
        void init();
        void renderLoop();
        void cleanup();

        std::shared_ptr<Window> window = nullptr;
        std::shared_ptr<RenderDevice> renderer = nullptr;
		std::shared_ptr<RenderPipeline> pipeline = nullptr;

        bool error = false;
    };
}