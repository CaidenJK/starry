#include "Renderer.h"

#include <stdexcept>
#include <iostream>

namespace StarryRender {
	Renderer::Renderer(const char* name) : name(name) {
		initVulkan();
	}

	Renderer::~Renderer() {
		vkDestroyInstance(instance, nullptr);
	}

	void Renderer::initVulkan() {
		createInstance();
		if (error) {
			return;
		}

		checkExtensions();
	}

	void Renderer::checkExtensions() {
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		std::cout << "Available extensions:";
		if (extensions.size() == 0 || extensionCount == 0) {
			std::cout << "\tNo extensions found.";
		}
		else {
			std::cout << "\n";
			for (const auto& extension : extensions) {
				std::cout << '\t' << extension.extensionName << '\n';
			}
		}
		std::cout << std::endl;
	}

	void Renderer::createInstance() {
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = name;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		
		createInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			error = true;
#ifdef SUCCESS_VALIDATION
			std::cerr << "Vulkan initialization failed!" << std::endl;
#endif
			return;
		}
	}

}