#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#define DEFULAT_NAME "Starry Driver"

namespace StarryRender {
	class Renderer {
	public:
		Renderer(const char* name = DEFULAT_NAME);
		~Renderer();

		bool getError() { return error; }

	private:
		void initVulkan();
		void createInstance();
		void checkExtensions();

		const char* name;
		std::vector<VkExtensionProperties> extensions;
		
		VkInstance instance;
		VkApplicationInfo appInfo{};
		VkInstanceCreateInfo createInfo{};

		bool error = false;
	};
}