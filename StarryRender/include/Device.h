#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <memory>

#include <StarryManager.h>

#include "Window.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "UniformBuffer.h"
#include "TextureImage.h"
#include "Descriptor.h"

namespace StarryRender
{
	class VulkanDebugger : public StarryAsset {
	public:
		void registerDebugAlert(const std::string& message, CallSeverity severity);

		ASSET_NAME("Vulkan Debugger")
	};

	struct DeviceConfig
	{
		VkSampleCountFlagBits desiredMSAASamples = VK_SAMPLE_COUNT_1_BIT;
		glm::vec3 clearColor;

		std::weak_ptr<Window> window;
	};

	struct DependencyConfig
	{
		int imageCount = 0;
	};

	struct DrawInfo
	{
		Pipeline& pipeline;
		SwapChain& swapChain;
		Descriptor& descriptor;

		std::weak_ptr<UniformBuffer>& uniformBuffer;
		std::weak_ptr<VertexBuffer>& vertexBuffer;
	};

	class Device : public StarryAsset {
		// Helper structs
		struct DeviceInfo {
			bool isSuitible = false;
			int score = 0;
			char name[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE] = "\0";
		};

	public:
		Device();
		~Device();

		Device operator=(const Device&) = delete;
		Device(const Device&) = delete;

		VkDevice& getDevice() { return m_device; }
		VkPhysicalDevice& getPhysicalDevice() { return m_physicalDevice; }
		VkSurfaceKHR& getSurface() { return m_surface; }
		QueueFamilyIndices& getQueueFamilies() { return m_queueFamilyIndices; }

		DeviceConfig& getConfig() { return m_config; }

		void init(DeviceConfig config);
		void destroy();

		//void loadShader(std::shared_ptr<Shader>& shaderRef);
		//void loadDescriptor(std::shared_ptr<Descriptor>& descriptor);
		//void loadBuffer(std::shared_ptr<VertexBuffer> buffer);

		void createDependencies(DependencyConfig config); // Can call when need to update
		void draw(DrawInfo info);

		void waitIdle();

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer& commandBuffer);

		ASSET_NAME("Render Device")

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData);

	private:
		bool m_toUpdate;

		void initVulkan();
		void createInstance();
		void checkVKExtensions();
		void createLogicalDevice();

		void createSwapChain();

		void createCommmandPool(DependencyConfig& config);
		void createCommandBuffers(DependencyConfig& config);
		void createSyncObjects(DependencyConfig& config);

		void recordCommandBuffer(DrawInfo& info, VkCommandBuffer commandBuffer, uint32_t imageIndex);

		std::vector<const char*> getRequiredGLFWExtensions();
		void checkValidationLayerSupport();

		void setupDebugMessenger();
		static void debugMessengerCreateInfoFactory(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void createSurface();

		void pickPhysicalDevice();
		DeviceInfo isDeviceSuitable(VkPhysicalDevice physicalDevice);
		VkSampleCountFlagBits getMaxUsableSampleCount();

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
		bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

		DeviceConfig m_config = {};

		std::vector<VkExtensionProperties> m_vkExtensions;
		const std::vector<const char*> m_validationLayers = {
			"VK_LAYER_KHRONOS_validation",
		};
		bool m_enableValidationLayers = false;



#ifdef __APPLE__
		const std::vector<const char*> m_instanceExtensions = {
			VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
			"VK_KHR_get_physical_device_properties2",
			"VK_EXT_layer_settings"
		};

		const std::vector<const char*> m_deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			"VK_KHR_portability_subset",
			"VK_KHR_surface"
		};
#else
		const std::vector<const char*> m_instanceExtensions;

		const std::vector<const char*> m_deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			"VK_KHR_surface"
		};
#endif
		VkInstance m_instance = VK_NULL_HANDLE;
		
		VkSurfaceKHR m_surface = VK_NULL_HANDLE;

		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkDevice m_device = VK_NULL_HANDLE;

		QueueFamilyIndices m_queueFamilyIndices;

		VkQueue m_graphicsQueue = VK_NULL_HANDLE;
		VkQueue m_presentQueue = VK_NULL_HANDLE;

		VkCommandPool m_commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_commandBuffers = {};

		// Presentation
		std::vector<VkSemaphore> m_imageAvailableSemaphores = {};
		std::vector<VkSemaphore> m_renderFinishedSemaphores = {};
		std::vector<VkFence> m_inFlightFences = {};

		uint32_t m_currentFrame = 0;

		VkPhysicalDeviceMemoryProperties m_memProperties = {};

		VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
		static VulkanDebugger* debugger;
	};
}

//TODO Better Logging