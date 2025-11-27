#include "RenderDevice.h"
#include <vulkan/vk_enum_string_helper.h>

#include <iostream>
#include <map>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION

#define THROW_ERROR(msg) \
	error = true; \
	std::cerr << "Device ERROR: " << msg << std::endl; \
	return

#define THROW_ERROR_RETURN(msg, x) \
	error = true; \
	std::cerr << "Device ERROR: " << msg << std::endl; \
	return x


#define ALERT_MSG(msg) \
	std::cout << msg

#else
#define THROW_ERROR(msg) \
	error = true; \
	return

#define ALERT_MSG(msg)

#define THROW_ERROR_RETURN(msg, x) \
	error = true; \
	return x

#endif

#define ERROR_VOLATILE(x) x; if (error) { return; }

#define START_WEAK_PTR \
	if (std::shared_ptr<Window> window = windowReference.lock()) {
#define END_WEAK_PTR(x) \
	} else { \
		THROW_ERROR_RETURN("Window reference is expired!", x); \
	}

namespace StarryRender {

	// Debug messenger proxy functions
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	void RenderDevice::debugMessengerCreateInfoFactory(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	RenderDevice::RenderDevice(std::shared_ptr<Window>& windowPointer, const char* name) : name(name) {
		windowReference = windowPointer;
		initVulkan();
	}

	RenderDevice::~RenderDevice() {
		// Future know where it errored as to clean up nessecary objects
		ERROR_VOLATILE();

		pipeline.reset();
		vkDestroySwapchainKHR(device, swapChain, nullptr);
		for (auto imageView : swapChainData.swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		if (commandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(device, commandPool, nullptr);
		}

		for (auto imageAvailableSemaphore : imageAvailableSemaphores) {
			vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		}
		for (auto renderFinishedSemaphore : renderFinishedSemaphores) {
			vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		}
		for (auto inFlightFence : inFlightFences) {
			vkDestroyFence(device, inFlightFence, nullptr);
		}

		vkDestroyDevice(device, nullptr);

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySurfaceKHR(instance, surface, nullptr);

		vkDestroyInstance(instance, nullptr);
	}

	void RenderDevice::initVulkan() {
		ERROR_VOLATILE(checkValidationLayerSupport());

		ERROR_VOLATILE(createInstance());

		ERROR_VOLATILE(setupDebugMessenger());
		checkVKExtensions();

		ERROR_VOLATILE(createSurface());

		ERROR_VOLATILE(pickPhysicalDevice());
		ERROR_VOLATILE(createLogicalDevice());
		
		ERROR_VOLATILE(createSwapChain());
		ERROR_VOLATILE(createImageViews());
	}

	void RenderDevice::setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		debugMessengerCreateInfoFactory(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			THROW_ERROR("Could not setup debug messenger!");
		}
	}

	void RenderDevice::checkValidationLayerSupport() {
#ifndef NDEBUG
		enableValidationLayers = true;
#endif

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				THROW_ERROR("Validation layer requested not available!");
			}
		}
	}

	void RenderDevice::checkVKExtensions() {
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		vkExtensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vkExtensions.data());

		ALERT_MSG("Avalible Vulkan Extensions: ");
		if (extensionCount == 0) {
			ALERT_MSG("\tNo extensions found.");
		}
		else {
			ALERT_MSG("\n");
			for (const auto& extension : vkExtensions) {
				ALERT_MSG('\t' << extension.extensionName << '\n');
			}
		}
		ALERT_MSG(std::endl);
	}

	std::vector<const char*> RenderDevice::getRequiredGLFWExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void RenderDevice::createInstance() {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = name;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			debugMessengerCreateInfoFactory(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		auto extensions = getRequiredGLFWExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			THROW_ERROR("Vulkan instance creation failed!");
		}
	}

	void RenderDevice::createSurface() {
		START_WEAK_PTR
			window->createVulkanSurface(instance, surface);
			error = window->getError();
		END_WEAK_PTR()
	}

	void RenderDevice::pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			THROW_ERROR("Failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		ALERT_MSG("Available Vulkan Devices: \n");
		std::multimap<int, VkPhysicalDevice> candidates;
		for (const auto& device : devices) {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			DeviceInfo deviceInfo = isDeviceSuitable(device);

			if (deviceInfo.isSuitible) {
				ALERT_MSG("\t" << deviceInfo.name << ", score: " << deviceInfo.score << "\n");
				candidates.insert(std::make_pair(deviceInfo.score, device));
			}
		}

		if (candidates.rbegin()->first < 1) {
			THROW_ERROR("Failed to find a suitable GPU!");
		}
		physicalDevice = candidates.rbegin()->second;
	}

	RenderDevice::QueueFamilyIndices RenderDevice::findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		// Find a queue that supports VK_QUEUE_GRAPHICS_BIT
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}
			
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			if (indices.isComplete()) { break; }
			i++;
		}

		return indices;
	}

	bool RenderDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		if (!requiredExtensions.empty()) {
			ALERT_MSG("Missing Device Extensions: \n");
			for (const auto& ext : requiredExtensions) {
				ALERT_MSG('\t' << ext << '\n');
			}
			ALERT_MSG(std::endl);
			return false;
		}
		return true;
	}

	RenderDevice::DeviceInfo RenderDevice::isDeviceSuitable(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		DeviceInfo info;
		QueueFamilyIndices indices = findQueueFamilies(device);
		info.score = 1;
		strcpy_s(info.name, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, deviceProperties.deviceName);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			info.score += 1000;
		}
		
		// Texture max size
		info.score += deviceProperties.limits.maxImageDimension2D;

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			// At least one of each
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		info.isSuitible = indices.isComplete() && extensionsSupported && swapChainAdequate && info.score > 0;
		return info;
	}

	RenderDevice::SwapChainSupportDetails RenderDevice::querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR RenderDevice::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		VkSurfaceFormatKHR currentSwapSurfaceFormat;
		bool isSet = false;
		for (const auto& availableFormat : availableFormats) {
			// SRGB colorspace
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				currentSwapSurfaceFormat = availableFormat;
				isSet = true;
				break;
			}
		}
		if (!isSet) {
			currentSwapSurfaceFormat = availableFormats[0];
		}
		ALERT_MSG("Chosen Swap Surface Format: " << string_VkFormat(currentSwapSurfaceFormat.format) << ", Color Space: " << string_VkColorSpaceKHR(currentSwapSurfaceFormat.colorSpace) << std::endl);
		return currentSwapSurfaceFormat;
	}
	VkPresentModeKHR RenderDevice::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&availablePresentModes) {
		VkPresentModeKHR currentPresentMode;
		bool isSet = false;
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				currentPresentMode = availablePresentMode;
				isSet = true;
				break;
			}
		}

		if (!isSet) {
			currentPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		}
		ALERT_MSG("Chosen Present Mode: " << string_VkPresentModeKHR(currentPresentMode) << std::endl);
		return currentPresentMode;
	}
	VkExtent2D RenderDevice::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			START_WEAK_PTR
				window->getFramebufferSize(width, height);
			END_WEAK_PTR({})

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	void RenderDevice::createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		// No device specific extensions needed yet

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			THROW_ERROR("Failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

	void RenderDevice::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		ERROR_VOLATILE(VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities));

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		// Ignore alpha
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		// Needed for resizing. No resizing
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			THROW_ERROR("Failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainData.swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainData.swapChainImages.data());


		swapChainData.swapChainImageFormat = surfaceFormat.format;
		swapChainData.swapChainExtent = extent;
	}

	void RenderDevice::createImageViews() {
		swapChainData.swapChainImageViews.resize(swapChainData.swapChainImages.size());

		for (size_t i = 0; i < swapChainData.swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainData.swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainData.swapChainImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainData.swapChainImageViews[i]) != VK_SUCCESS) {
				THROW_ERROR("Failed to create image views!");
			}
		}
	}

	void RenderDevice::setPipeline(std::shared_ptr<RenderPipeline>& pipelineTarget) {
		pipeline = pipelineTarget;
	}

	void RenderDevice::setPipeline(const std::string& vertShader, const std::string& fragShader) {
		if (!instance) {
			THROW_ERROR("Vulkan instance not initialized! Can't set pipeline.");
		}
		pipeline = std::make_shared<RenderPipeline>(vertShader, fragShader, device, swapChainData);
		error = pipeline->getError();
	}

	void RenderDevice::Init() {
		ERROR_VOLATILE()
		if (commandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(device, commandPool, nullptr);
		}
		for (auto imageAvailableSemaphore : imageAvailableSemaphores) {
			vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		}
		imageAvailableSemaphores.clear();
		for (auto renderFinishedSemaphore : renderFinishedSemaphores) {
			vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		}
		renderFinishedSemaphores.clear();
		for (auto inFlightFence : inFlightFences) {
			vkDestroyFence(device, inFlightFence, nullptr);
		}
		inFlightFences.clear();

		ERROR_VOLATILE(createCommmandPool());
		ERROR_VOLATILE(createCommandBuffers());

		ERROR_VOLATILE(createSyncObjects());
	}

	void RenderDevice::createCommmandPool() {
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			THROW_ERROR("Failed to create command pool!");
		}
	}

	void RenderDevice::createCommandBuffers() {
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();;

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			THROW_ERROR("Failed to allocate command buffers!");
		}
	}

	void RenderDevice::createSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(swapChainData.swapChainImages.size());
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

				THROW_ERROR("Failed to create synchronization objects for all frames!");
			}
		}
		for (size_t i = 0; i < swapChainData.swapChainImages.size(); i++) {
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
				THROW_ERROR("Failed to create synchronization objects for all frames!");
			}
		}
	}

	// For each command buffer
	void RenderDevice::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			THROW_ERROR("Failed to begin recording command buffer");
		}

		if (pipeline == nullptr) {
			THROW_ERROR("Pipeline not created or stored before recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipeline->getRenderPass();
		renderPassInfo.framebuffer = pipeline->getFramebuffers()[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainData.swapChainExtent;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Start of recording
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getGraphicsPipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainData.swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainData.swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainData.swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		// End
		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			THROW_ERROR("Failed to record command buffer!");
		}
	}

	void RenderDevice::Draw() {
		ERROR_VOLATILE();
		if (pipeline == nullptr ||
			commandPool == VK_NULL_HANDLE ||
			imageAvailableSemaphores.size() == 0 ||
			renderFinishedSemaphores.size() == 0 ||
			inFlightFences.size() == 0) {
			THROW_ERROR("Draw called before Device was fully initilized.");
		}

		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		// Aquire image from swapchain
		uint32_t imageIndex;
		vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[imageIndex]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			THROW_ERROR("Failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(presentQueue, &presentInfo);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void RenderDevice::WaitIdle() {
		vkDeviceWaitIdle(device);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL RenderDevice::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "Vulkan Debug Message..." << std::endl;
		std::cerr << "Validation layer: " << pCallbackData->pMessage << "\n" << std::endl;

		return VK_FALSE;
	}
}