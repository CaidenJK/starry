#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>

#include "Window.h"

#define DEFAULT_NAME "My Starry App"

namespace StarryRender {
	class RenderDevice {
		// Helper structs
		struct DeviceInfo {
			bool isSuitible;
			int score = 0;
			char name[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
		};

		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete() {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};


	public:
		RenderDevice(Window*& windowReference, const char* name = DEFAULT_NAME);
		~RenderDevice();

		bool getError() { return error; }
		VkInstance getInstance() { return instance; }

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
		void createLogicalDevice();

		std::vector<const char*> getRequiredGLFWExtensions();
		void checkValidationLayerSupport();

		void setupDebugMessenger();
		static void debugMessengerCreateInfoFactory(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void createSurface();

		void pickPhysicalDevice();
		DeviceInfo isDeviceSuitable(VkPhysicalDevice device);

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

		const char* name;
		
		std::vector<VkExtensionProperties> vkExtensions;
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		bool enableValidationLayers = false;
		
		VkInstance instance;
		VkApplicationInfo appInfo{};
		VkInstanceCreateInfo createInfo{};
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkDebugUtilsMessengerEXT debugMessenger;

		// Window outlives RenderDevice for now
		Window*& windowReference;

		bool error = false;
	};
}

//TODO Better Logging