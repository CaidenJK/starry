#pragma once

#include "Window.h"
#include "RenderDevice.h"

#include <iostream>

#define STARRY_APP_VERSION "0.0.1"
 
namespace StarryRender {
    class Application {
    public:
        Application() {};
        ~Application() {};

        void run() {
            printVersion();
            initVulkan();
            renderLoop();
            cleanup();
        }

    private:
        void printVersion() {
            std::cout << "Hello! Starry Application Version: " << STARRY_APP_VERSION << std::endl;
        }
        void initVulkan();
        void renderLoop();
        void cleanup();

        Window* window = nullptr;
        RenderDevice* renderer = nullptr;
    };
}