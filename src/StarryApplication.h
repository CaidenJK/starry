#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#define STARRY_APP_VERSION "1.0.0"

class StarryApplication {
public:
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
};