#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <memory>

#include <StarryAsset.h>

#include "Window.h"
#include "RenderPipeline.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "UniformBuffer.h"

#define DEFAULT_NAME "My Starry App"

namespace StarryRender
{
	class VulkanDebugger : public StarryAsset {
		public:
			void registerDebugAlert(const std::string& message, CallSeverity severity);

			const std::string getAssetName() override {return "Vulkan Debugger";}
	};

	class RenderDevice : public StarryAsset {
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

		void loadUniformBuffer(std::shared_ptr<UniformBuffer>& bufferRef);
		void loadUniformBuffer(std::unique_ptr<UniformBuffer>& bufferRef);

		void LoadVertexBuffer(std::shared_ptr<VertexBuffer>& bufferRef);

		void LoadShader(const std::string& vertShader, const std::string& fragShader);
		void LoadShader(std::shared_ptr<Shader>& shaderRef);
		void LoadPipeline(std::shared_ptr<RenderPipeline>& pipelineRef);
		void InitDraw();
		void Draw();

		void WaitIdle();

		VkDevice& getDevice() { return device; }
		VkPhysicalDevice& getPhysicalDevice() { return physicalDevice; }
		VkExtent2D getExtent() { return swapChain->getExtent(); }

		const std::string getAssetName() override { return "RenderDevice"; }

		enum SharedResources {
			VK_DEVICE = 0,
			VK_PHYSICAL_DEVICE = 1,
			SWAP_CHAIN_IMAGE_FORMAT = 2,
			UNIFORM_BUFFER = 3,
			WINDOW_REFERENCE = 4
		};

		std::optional<void*> getResource(size_t resourceID) override;
		size_t getResourceIDFromString(std::string resourceName) override;

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



#ifdef __APPLE__
		const std::vector<const char*> instanceExtensions = {
			VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
			"VK_KHR_get_physical_device_properties2"
		};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			"VK_KHR_portability_subset"
		};
#else
		const std::vector<const char*> instanceExtensions;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
#endif
		
		VkInstance instance = VK_NULL_HANDLE;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;

		VkCommandPool commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers = {};
		
		// Presentation
		std::vector<VkSemaphore> imageAvailableSemaphores = {};
		std::vector<VkSemaphore> renderFinishedSemaphores = {};
		std::vector<VkFence> inFlightFences = {};

		uint32_t currentFrame = 0;

		// Dyno resolution
		VkExtent2D drawExtent = {};
		float renderScale = 1.0f;

		std::shared_ptr<VertexBuffer> vertexBuffer = nullptr;
		std::weak_ptr<UniformBuffer> uniformBuffer = {};

		VkPhysicalDeviceMemoryProperties memProperties = {};

		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

		// Window outlives RenderDevice for now
		std::weak_ptr<Window> windowReference = {};

		std::shared_ptr<RenderPipeline> pipeline = nullptr;
		std::shared_ptr<SwapChain> swapChain = nullptr;

		static VulkanDebugger* debugger;
	};
}

//TODO Better Logging