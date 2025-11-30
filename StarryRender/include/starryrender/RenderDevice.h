#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <memory>

#include "Asset.h"
#include "Window.h"
#include "RenderPipeline.h"
#include "SwapChain.h"
#include "VertexBuffer.h"

#define DEFAULT_NAME "My Starry App"

namespace StarryRender {
	class RenderDevice : public RenderAsset {
		// Helper structs
		struct DeviceInfo {
			bool isSuitible = false;
			int score = 0;
			char name[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE] = "\0";
		};

	public:
		RenderDevice(std::shared_ptr<Window>& windowPointer, const char* name = DEFAULT_NAME);
		~RenderDevice();

		RenderDevice operator=(const RenderDevice&) = delete;
		RenderDevice(const RenderDevice&) = delete;

		VkInstance getInstance() { return instance; }

		void LoadBuffer(std::shared_ptr<VertexBuffer>& bufferRef);

		void LoadShader(const std::string& vertShader, const std::string& fragShader);
		void LoadShader(std::shared_ptr<Shader>& shaderRef);
		void LoadPipeline(std::shared_ptr<RenderPipeline>& pipelineRef);
		void InitDraw();
		void Draw();

		void WaitIdle();

		VkDevice& getDevice() { return device; }

		const std::string getAssetName() override { return "RenderDevice"; }

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

		void createCommmandPool();
		void createCommandBuffers();
		void createSyncObjects();

		void recreateSwapChain();

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

		void constructDefaultTriangle();

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

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkCommandPool commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers = {};
		
		// Presentation
		std::vector<VkSemaphore> imageAvailableSemaphores = {};
		std::vector<VkSemaphore> renderFinishedSemaphores = {};
		std::vector<VkFence> inFlightFences = {};

		uint32_t currentFrame = 0;

		// Dyno resolution
		VkExtent2D drawExtent;
		float renderScale = 1.0f;

		std::shared_ptr<VertexBuffer> vertexBuffer = nullptr;

		VkPhysicalDeviceMemoryProperties memProperties;

		VkDebugUtilsMessengerEXT debugMessenger;

		// Window outlives RenderDevice for now
		std::weak_ptr<Window> windowReference;

		std::shared_ptr<RenderPipeline> pipeline = nullptr;
		std::shared_ptr<SwapChain> swapChain = nullptr;
	};
}

//TODO Better Logging