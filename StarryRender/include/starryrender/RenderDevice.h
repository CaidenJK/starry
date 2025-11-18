#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#define DEFULAT_NAME "Starry Driver"

namespace StarryRender {
	class RenderDevice {
	public:
		RenderDevice(const char* name = DEFULAT_NAME);
		~RenderDevice();

		bool getError() { return error; }

		// Error Handler can use this
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	private:
		void initVulkan();
		void createInstance();
		void checkVKExtensions();

		std::vector<const char*> getRequiredGLFWExtensions();
		void checkValidationLayerSupport();

		void setupDebugMessenger();

		static void debugMessengerCreateInfoFactory(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		const char* name;
		
		std::vector<VkExtensionProperties> vkExtensions;
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		bool enableValidationLayers = false;
		
		VkInstance instance;
		VkApplicationInfo appInfo{};
		VkInstanceCreateInfo createInfo{};
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		VkDebugUtilsMessengerEXT debugMessenger;

		bool error = false;
	};
}

//TODO Better Logging