#pragma once

#include "Window.h"
#include "Scene.h"

#include <iostream>
#include <memory>
#include <thread>
#include <atomic>

#define STARRY_APP_VERSION "0.0.2 dev"
 
namespace StarryRender 
{
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

        std::shared_ptr<Window> window = nullptr;
        std::shared_ptr<Scene> scene = nullptr;
    };
}