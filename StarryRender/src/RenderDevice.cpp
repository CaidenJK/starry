#include "RenderDevice.h"
#include <vulkan/vk_enum_string_helper.h>

#include <map>
#include <set>
#include <cstdint>
#include <string>
#include <cstring>

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; } 

#define START_WINDOW_PTR \
	if (std::shared_ptr<Window> window = windowReference.lock()) {

#define END_WINDOW_PTR(x) \
	} else { \
		registerAlert("Window reference is expired!", FATAL); \
		return x; \
	}

namespace StarryRender 
{
	VulkanDebugger* RenderDevice::debugger = nullptr;
	void VulkanDebugger::registerDebugAlert(const std::string& message, CallSeverity severity) {
		registerAlert(message, severity);
	}

	// Debug messenger proxy functions
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	void RenderDevice::debugMessengerCreateInfoFactory(VkDebugUtilsMessengerCreateInfoEXT& createInfo) 
	{	
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
	}

	RenderDevice::RenderDevice(std::shared_ptr<Window>& windowPointer, RenderConfig& config, const char* name) : name(name), config(config)
	{
		if (windowPointer == nullptr) {
			registerAlert("Window pointer is null!", FATAL);
			return;
		}
		windowReference = windowPointer;
#ifndef NDEBUG
		enableValidationLayers = true;
		debugger = new VulkanDebugger();
#else
		debugger = nullptr;
		enableValidationLayers = false;
#endif

		initVulkan();
	}

	RenderDevice::~RenderDevice() 
	{
		pipeline.reset();
		swapChain.reset();
		descriptor.reset();

		if (auto cnvs = canvas.lock()) {
			cnvs->Destroy();
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

		vkDestroyDevice(device, nullptr); // ---- DEVICE DESTRUCTION ----

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySurfaceKHR(instance, surface, nullptr);

		vkDestroyInstance(instance, nullptr); // ---- INSTANCE DESTRUCTION ----

		delete debugger;
	}

	std::optional<void*> RenderDevice::getResource(size_t resourceID, std::vector<size_t> resourceArgs)
	{
		if (resourceID == SharedResources::VK_DEVICE &&
			device != VK_NULL_HANDLE) {
			return (void*)&device;
		}
		if (resourceID == SharedResources::VK_PHYSICAL_DEVICE &&
			physicalDevice != VK_NULL_HANDLE) {
				return (void*)&physicalDevice;
			}
		if (resourceID == SharedResources::SWAP_CHAIN_IMAGE_FORMAT &&
			swapChain != nullptr) {
			return (void*)&(swapChain->getImageFormats());
		}
		if (resourceID == SharedResources::DESCRIPTOR &&
			descriptor != nullptr) {
			return (void*)&descriptor;
		}
		if (resourceID == SharedResources::WINDOW_REFERENCE &&
			!windowReference.expired()) {
			return (void*)&windowReference;
		}
		if (resourceID == SharedResources::COMMAND_POOL &&
			commandPool != VK_NULL_HANDLE) {
			return (void*)&commandPool;
		}
		if (resourceID == SharedResources::GRAPHICS_QUEUE &&
			graphicsQueue != VK_NULL_HANDLE) {
			return (void*)&graphicsQueue;
		}
		if (resourceID == SharedResources::SWAP_CHAIN_EXTENT &&
			swapChain != nullptr) {
			return (void*)&swapChain->getExtent();
		}
		if (resourceID == SharedResources::MSAA_SAMPLES) {
			return (void*)&msaaSamples;
		}

		registerAlert(std::string("No matching resource: ") + std::to_string(resourceID) + " available for sharing", WARNING);
		return {};
	}

	size_t RenderDevice::getResourceIDFromString(const std::string resourceName)
	{
		if (resourceName.compare("VkDevice") == 0) {
			return SharedResources::VK_DEVICE;
		}
		if (resourceName.compare("Physical Device") == 0) {
			return SharedResources::VK_PHYSICAL_DEVICE;
		}
		if (resourceName.compare("Swapchain Image Formats") == 0) {
			return SharedResources::SWAP_CHAIN_IMAGE_FORMAT;
		}
		if (resourceName.compare("Descriptor") == 0) {
			return SharedResources::DESCRIPTOR;
		}
		if (resourceName.compare("Window") == 0) {
			return SharedResources::WINDOW_REFERENCE;
		}
		if (resourceName.compare("Command Pool") == 0) {
			return SharedResources::COMMAND_POOL;
		}
		if (resourceName.compare("Graphics Queue") == 0) {
			return SharedResources::GRAPHICS_QUEUE;
		}
		if (resourceName.compare("Extent") == 0) {
			return SharedResources::SWAP_CHAIN_EXTENT;
		}
		if (resourceName.compare("MSAA Samples") == 0) {
			return SharedResources::MSAA_SAMPLES;
		}
		return INVALID_RESOURCE;
	}

	ImGUI_Config RenderDevice::getImGUIConfig()
	{
		if (!descriptor) {
			registerAlert("Config requested before device was initilized!", CRITICAL);
			return {};
		}
		ImGui_ImplVulkan_InitInfo init_info = {};
		//init_info.ApiVersion = VK_API_VERSION_1_3;
		init_info.Instance = instance;
		init_info.PhysicalDevice = physicalDevice;
		init_info.Device = device;
		init_info.QueueFamily = findQueueFamilies(physicalDevice).presentFamily.value();
		init_info.Queue = presentQueue;
		init_info.PipelineCache = nullptr;
		init_info.DescriptorPool = descriptor->getPool();
		init_info.MinImageCount = 2;
		init_info.ImageCount = swapChain->getImageCount();
		init_info.Allocator = nullptr;
		init_info.PipelineInfoMain.RenderPass = pipeline->getRenderPass();
		init_info.PipelineInfoMain.Subpass = 0;
		init_info.PipelineInfoMain.MSAASamples = msaaSamples;
		init_info.CheckVkResultFn = nullptr;

		return ImGUI_Config(windowReference, init_info);
	}

	void RenderDevice::loadCanvas(std::shared_ptr<Canvas>& canvasRef)
	{
		canvasRef->registerCanvas(getImGUIConfig());
		canvas = canvasRef;
	}

	void RenderDevice::initVulkan() 
	{
		ERROR_VOLATILE(checkValidationLayerSupport());

		ERROR_VOLATILE(createInstance());

		ERROR_VOLATILE(setupDebugMessenger());
		checkVKExtensions();
	
		ERROR_VOLATILE(createSurface());

		ERROR_VOLATILE(pickPhysicalDevice());
		ERROR_VOLATILE(createLogicalDevice());
	
		ERROR_VOLATILE(createSwapChain());

		descriptor = std::make_shared<Descriptor>();
	}

	void RenderDevice::setupDebugMessenger() 
	{
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		debugMessengerCreateInfoFactory(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			registerAlert("Could not setup debug messenger!", FATAL);
			return;
		}
	}

	void RenderDevice::checkValidationLayerSupport() 
	{
		if (!enableValidationLayers) return;

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
				registerAlert("Validation layer requested not available!", FATAL);
				return;
			}
		}
	}

	void RenderDevice::checkVKExtensions() 
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		vkExtensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vkExtensions.data());

		std::string message = "Avalible Vulkan Extensions: \n";
		if (extensionCount == 0) {
			message += "\tNo extensions found.";
		}
		else {
			for (const auto& extension : vkExtensions) {
				message += '\t' + std::string(extension.extensionName) + '\n';
			}
		}
		registerAlert(message, INFO);
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

	void RenderDevice::createInstance() 
	{
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
#ifdef __APPLE__
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			const VkBool32 verbose_value = true;
			const VkLayerSettingEXT layerSetting = {"VK_LAYER_KHRONOS_validation", "validate_best_practices", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &verbose_value};
			VkLayerSettingsCreateInfoEXT layerSettingsCreateInfo = {VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT, nullptr, 1, &layerSetting};

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			debugMessengerCreateInfoFactory(debugCreateInfo);
			debugCreateInfo.pNext = &layerSettingsCreateInfo;
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		auto extensions = getRequiredGLFWExtensions();
		for (const auto& exten : instanceExtensions) {
			extensions.emplace_back(exten);
		}

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			registerAlert("Vulkan instance creation failed!", FATAL);
			return;
		}
	}

	void RenderDevice::createSurface() 
	{
		START_WINDOW_PTR
			window->createVulkanSurface(instance, surface);
			EXTERN_ERROR(window);
		END_WINDOW_PTR()
	}

	void RenderDevice::pickPhysicalDevice() 
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			registerAlert("Failed to find GPUs with Vulkan support!", FATAL);
			return;
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		std::string messsage = "Available Vulkan Devices: \n";
		std::multimap<int, VkPhysicalDevice> candidates;
		for (const auto& device : devices) {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			DeviceInfo deviceInfo = isDeviceSuitable(device);

			if (deviceInfo.isSuitible) {
				messsage += "\t" + std::string(deviceInfo.name) + ", score: " + std::to_string(deviceInfo.score) + "\n";
				candidates.insert(std::make_pair(deviceInfo.score, device));
			}
		}

		if (candidates.empty() || candidates.rbegin()->first < 1) {
			registerAlert("Failed to find a suitable GPU!", FATAL);
			return;
		}
		registerAlert(messsage, INFO);
		physicalDevice = candidates.rbegin()->second;
		msaaSamples = getMaxUsableSampleCount();
	}

	QueueFamilyIndices RenderDevice::findQueueFamilies(VkPhysicalDevice device) 
	{
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

	bool RenderDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) 
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		if (!requiredExtensions.empty()) {
			std::string message = "Device missing extensions: \n";
			for (const auto& ext : requiredExtensions) {
				message += '\t' + ext + '\n';
			}
			registerAlert(message, WARNING);
			return false;
		}
		return true;
	}

	VkSampleCountFlagBits RenderDevice::getMaxUsableSampleCount() {
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		VkSampleCountFlagBits result;

		if (counts & VK_SAMPLE_COUNT_64_BIT) { result = VK_SAMPLE_COUNT_64_BIT; }
		else if (counts & VK_SAMPLE_COUNT_32_BIT) { result = VK_SAMPLE_COUNT_32_BIT; }
		else if (counts & VK_SAMPLE_COUNT_16_BIT) { result = VK_SAMPLE_COUNT_16_BIT; }
		else if (counts & VK_SAMPLE_COUNT_8_BIT) { result = VK_SAMPLE_COUNT_8_BIT; }
		else if (counts & VK_SAMPLE_COUNT_4_BIT) { result = VK_SAMPLE_COUNT_4_BIT; }
		else if (counts & VK_SAMPLE_COUNT_2_BIT) { result = VK_SAMPLE_COUNT_2_BIT; }
		else { result = VK_SAMPLE_COUNT_1_BIT; }

		return std::min(result, config.desiredMSAASamples);
	}

	RenderDevice::DeviceInfo RenderDevice::isDeviceSuitable(VkPhysicalDevice device) 
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		DeviceInfo info;
		QueueFamilyIndices indices = findQueueFamilies(device);
		info.score = 1;
		std::strncpy(info.name, deviceProperties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			info.score += 1000;
		}
		
		// Texture max size
		info.score += deviceProperties.limits.maxImageDimension2D;

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChain::SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(device, surface);
			// At least one of each
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
    	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		info.isSuitible = indices.isComplete() && extensionsSupported && swapChainAdequate && info.score > 0 &&
		supportedFeatures.samplerAnisotropy;
		return info;
	}


	void RenderDevice::createLogicalDevice() 
	{
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
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE;

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
			registerAlert("Failed to create logical device!", FATAL);
			return;
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}
	void RenderDevice::createSwapChain() 
	{
		if (!device) {
			registerAlert("Vulkan device not initialized! Can't create swapchain.", FATAL);
			return;
		}

		swapChain = std::make_shared<SwapChain>();
		SwapChain::SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(physicalDevice, surface);
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		swapChainSupport.msaaSamples = msaaSamples;

		swapChain->constructSwapChain(swapChainSupport, indices, surface);
		EXTERN_ERROR(swapChain);
	}

	void RenderDevice::loadUniformBuffer(std::shared_ptr<UniformBuffer>& bufferRef) 
	{
		uniformBuffer = bufferRef;
	}

	void RenderDevice::loadImageBuffer(std::shared_ptr<TextureImage>& bufferRef) 
	{
		textureImage = bufferRef;
	}

	void RenderDevice::setDescriptors()
	{
		auto ub = uniformBuffer.lock();
		auto ib = textureImage.lock();

		if (ub && ib) {
			descriptor->createSets(ub->getUUID(), ib->getUUID());
		} else {
			registerAlert("Buffer references are expired!", FATAL);
			return;
		}
	}

	void RenderDevice::LoadShader(const std::string& vertShader, const std::string& fragShader)
	{
		if (!device) {
			registerAlert("Vulkan device not initialized! Can't create pipeline with shader.", FATAL);
			return;
		}
		std::shared_ptr<Shader> shader = std::make_shared<Shader>(vertShader, fragShader);
		EXTERN_ERROR(shader);

		pipeline = std::make_shared<RenderPipeline>(shader);
		EXTERN_ERROR(pipeline);

		swapChain->generateFramebuffers(pipeline->getRenderPass());
		EXTERN_ERROR(swapChain);
	}

	void RenderDevice::LoadShader(std::shared_ptr<Shader>& shader)
	{
		if (!device) {
			registerAlert("Vulkan device not initialized! Can't create pipeline with shader.", FATAL);
			return;
		}
		EXTERN_ERROR(shader);

		pipeline = std::make_shared<RenderPipeline>(shader);
		EXTERN_ERROR(pipeline);

		swapChain->generateFramebuffers(pipeline->getRenderPass());
		EXTERN_ERROR(swapChain);
	}

	void RenderDevice::LoadPipeline(std::shared_ptr<RenderPipeline>& pipelineRef)
	{
		EXTERN_ERROR(pipelineRef);
		pipeline = pipelineRef;

		swapChain->generateFramebuffers(pipeline->getRenderPass());
		EXTERN_ERROR(swapChain);
	}

	void RenderDevice::InitDraw()
	{
		ERROR_VOLATILE();
		if (pipeline == nullptr) {
			registerAlert("Pipeline not created before Init!", FATAL);
			return;
		}
		if (swapChain == nullptr) {
			registerAlert("SwapChain not created before Init!", FATAL);
			return;
		}
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

	void RenderDevice::createCommmandPool() 
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			registerAlert("Failed to create command pool!", FATAL);
			return;
		}
	}

	void RenderDevice::createCommandBuffers() 
	{
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();;

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			registerAlert("Failed to allocate command buffers!", FATAL);
			return;
		}
	}

	void RenderDevice::createSyncObjects() 
	{
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(swapChain->getImageCount());
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

				registerAlert("Failed to create synchronization objects for all frames!", FATAL);
				return;
			}
		}
		for (size_t i = 0; i < swapChain->getImageCount(); i++) {
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
				registerAlert("Failed to create synchronization objects for all frames!", FATAL);
				return;
			}
		}
	}

	// For each command buffer
	void RenderDevice::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) 
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			registerAlert("Failed to begin recording command buffer", FATAL);
			return;
		}

		if (pipeline == nullptr) {
			registerAlert("Pipeline not created or stored before recording command buffer!", FATAL);
			return;
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipeline->getRenderPass();
		renderPassInfo.framebuffer = swapChain->getFramebuffers()[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->getExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Start of recording
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getGraphicsPipeline());
		
		size_t numberOfIndices = 0;
		if (auto vb = vertexBuffer.lock()) {
			VkBuffer vertexBuffers[] = { vb->getVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, vb->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			numberOfIndices = vb->getNumIndices();
		}
		else {
			registerAlert("Draw was called before loading a valid/alive Vertex Buffer.", CRITICAL);
			return;
		}

		if (auto ub = uniformBuffer.lock()) {
			ub->updateUniformBuffer(currentFrame);
		}
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipelineLayout(), 0, 1, &(descriptor->getDescriptorSet(currentFrame)), 0, nullptr);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChain->getExtent().width);
		viewport.height = static_cast<float>(swapChain->getExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChain->getExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(numberOfIndices), 1, 0, 0, 0);

		if (auto cnvs = canvas.lock()) {
			cnvs->Record(commandBuffer);
		}

		// End
		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			registerAlert("Failed to record command buffer!", FATAL);
			return;
		}
	}

	void RenderDevice::Draw() 
	{
		ERROR_VOLATILE();
		if (pipeline == nullptr ||
			swapChain == nullptr ||
			commandPool == VK_NULL_HANDLE ||
			imageAvailableSemaphores.size() == 0 ||
			renderFinishedSemaphores.size() == 0 ||
			inFlightFences.size() == 0) {
			registerAlert("Draw called before Device was fully initilized.", FATAL);
			return;
		}

		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		// Aquire image from swapchain
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device, swapChain->getSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			registerAlert("Failed to acquire swap chain image!", FATAL);
			return;
		}
		
		recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		//vkResetCommandBuffer(commandBuffers[currentFrame], 0);

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
			registerAlert("Failed to submit draw command buffer!", FATAL);
			return;
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		VkSwapchainKHR swapChains[] = { swapChain->getSwapChain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		bool framebufferResized = false;
		START_WINDOW_PTR
			framebufferResized = window->wasFramebufferResized();
		END_WINDOW_PTR()

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
			// try again next time
			START_WINDOW_PTR
				window->resetFramebufferResizedFlag();
			END_WINDOW_PTR()
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS) {
			registerAlert("Failed to acquire swap chain image!", FATAL);
			return;
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void RenderDevice::loadVertexBuffer(std::shared_ptr<VertexBuffer>& bufferRef)
	{
		if (bufferRef == nullptr) {
			registerAlert("Vertex buffer reference was null and was not set!", CRITICAL);
			return;
		}
		if (physicalDevice == VK_NULL_HANDLE || commandPool == VK_NULL_HANDLE || graphicsQueue == VK_NULL_HANDLE) {
			registerAlert("Render device not fully initialized! Can't load buffer to memory.", FATAL);
			return;
		}
		bufferRef->loadBufferToMemory();

		vertexBuffer = bufferRef;
	}

	void RenderDevice::WaitIdle() 
	{
		vkDeviceWaitIdle(device);
	}

	void RenderDevice::recreateSwapChain() 
	{
		START_WINDOW_PTR
			// Try again later
			if (window->isWindowMinimized()) { return; }
		END_WINDOW_PTR()
		vkDeviceWaitIdle(device);

		SwapChain::SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(physicalDevice, surface);
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		swapChainSupport.msaaSamples = msaaSamples;

		swapChain->constructSwapChain(swapChainSupport, indices, surface);
		EXTERN_ERROR(swapChain);
		swapChain->generateFramebuffers(pipeline->getRenderPass());
		EXTERN_ERROR(swapChain);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL RenderDevice::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (debugger == nullptr) return VK_FALSE;

		CallSeverity severity = INFO;
		if (messageSeverity & (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)) {
				severity = INFO_URGANT;
			}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			severity = WARNING;
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			severity = CRITICAL;
		}

		debugger->registerDebugAlert("Validation layer: " + std::string(pCallbackData->pMessage) + "\n", severity);
		return VK_FALSE;
	}
}