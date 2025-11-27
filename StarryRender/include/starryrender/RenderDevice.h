#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>
#include <string>
#include <memory>

#include "Window.h"
#include "RenderPipeline.h"

#define DEFAULT_NAME "My Starry App"

namespace StarryRender {
	class RenderDevice {
		// Helper structs
		struct DeviceInfo {
			bool isSuitible = false;
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
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

	public:
		RenderDevice(std::shared_ptr<Window>& windowPointer, const char* name = DEFAULT_NAME);
		~RenderDevice();

		bool getError() { return error; }
		VkInstance getInstance() { return instance; }

		void setPipeline(std::shared_ptr<RenderPipeline>& pipelineTarget);
		void setPipeline(const std::string& vertShader, const std::string& fragShader);

		void Init();

		void Draw();

		void WaitIdle();

		// Error Handler can use this
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	private:
		static const int MAX_FRAMES_IN_FLIGHT = 2;

		void initVulkan();
		void createInstance();
		void checkVKExtensions();
		void createLogicalDevice();
		void createSwapChain();
		void createImageViews();
		void createCommmandPool();
		void createCommandBuffers();
		void createSyncObjects();

		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		std::vector<const char*> getRequiredGLFWExtensions();
		void checkValidationLayerSupport();

		void setupDebugMessenger();
		static void debugMessengerCreateInfoFactory(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void createSurface();

		void pickPhysicalDevice();
		DeviceInfo isDeviceSuitable(VkPhysicalDevice device);

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		const char* name;
		
		std::vector<VkExtensionProperties> vkExtensions;
		const std::vector<const char*> validationLayers = { 
			"VK_LAYER_KHRONOS_validation" 
		};
		bool enableValidationLayers = false;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		
		VkInstance instance;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VkSwapchainKHR swapChain;
		SwapChainMetaData swapChainData;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkCommandPool commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers = {}; // iterate and destroy
		
		// Presentation
		std::vector<VkSemaphore> imageAvailableSemaphores = {};
		std::vector<VkSemaphore> renderFinishedSemaphores = {};
		std::vector<VkFence> inFlightFences = {};

		uint32_t currentFrame = 0;

		VkDebugUtilsMessengerEXT debugMessenger;

		// Window outlives RenderDevice for now
		std::weak_ptr<Window> windowReference;

		std::shared_ptr<RenderPipeline> pipeline = nullptr;

		bool error = false;
	};
}

//TODO Better Logging