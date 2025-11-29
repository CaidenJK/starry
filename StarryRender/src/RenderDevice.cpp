#include "RenderDevice.h"
#include <vulkan/vk_enum_string_helper.h>

#include <iostream>
#include <map>
#include <set>
#include <cstdint>

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION

#define ALERT_MSG(msg) \
	std::cout << msg

#else

#define ALERT_MSG(msg)

#endif

#define ERROR_VOLATILE(x) x; if (getError()) { return; }

#define EXTERN_ERROR(x) if(x->getError()) { return; } 

#define START_WEAK_PTR \
	if (std::shared_ptr<Window> window = windowReference.lock()) {

#define END_WEAK_PTR(x) \
	} else { \
		registerError("Window reference is expired!"); \
		return x; \
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
		swapChain.reset();

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
	}

	void RenderDevice::setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		debugMessengerCreateInfoFactory(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			registerError("Could not setup debug messenger!");
			return;
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
				registerError("Validation layer requested not available!");
				return;
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
			registerError("Vulkan instance creation failed!");
			return;
		}
	}

	void RenderDevice::createSurface() {
		START_WEAK_PTR
			window->createVulkanSurface(instance, surface);
			EXTERN_ERROR(window);
		END_WEAK_PTR()
	}

	void RenderDevice::pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			registerError("Failed to find GPUs with Vulkan support!");
			return;
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
			registerError("Failed to find a suitable GPU!");
			return;
		}
		physicalDevice = candidates.rbegin()->second;
	}

	QueueFamilyIndices RenderDevice::findQueueFamilies(VkPhysicalDevice device) {
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
			SwapChain::SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(device, surface);
			// At least one of each
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		info.isSuitible = indices.isComplete() && extensionsSupported && swapChainAdequate && info.score > 0;
		return info;
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
			registerError("Failed to create logical device!");
			return;
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

	void RenderDevice::createSwapChain() {
		if (!device) {
			registerError("Vulkan device not initialized! Can't create swapchain.");
			return;
		}

		swapChain = std::make_shared<SwapChain>(device);
		SwapChain::SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(physicalDevice, surface);
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		swapChain->constructSwapChain(swapChainSupport, indices, windowReference, surface);
		if (swapChain->getError()) { return; }
	}

	void RenderDevice::loadShader(const std::string& vertShader, const std::string& fragShader) {
		if (!device) {
			registerError("Vulkan device not initialized! Can't create pipeline with shader.");
			return;
		}
		std::shared_ptr<Shader> shader = std::make_shared<Shader>(device, vertShader, fragShader);
		EXTERN_ERROR(shader);

		pipeline = std::make_shared<RenderPipeline>(device);
		EXTERN_ERROR(pipeline);

		pipeline->loadShader(shader);
		EXTERN_ERROR(pipeline);

		pipeline->constructPipeline(swapChain->getImageFormat());
		EXTERN_ERROR(pipeline);

		swapChain->generateFramebuffers(pipeline->getRenderPass());
		EXTERN_ERROR(swapChain);
	}

	void RenderDevice::Init() {
		ERROR_VOLATILE();
		if (pipeline == nullptr) {
			registerError("Pipeline not created before Init!");
			return;
		}
		if (swapChain == nullptr) {
			registerError("SwapChain not created before Init!");
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

	void RenderDevice::createCommmandPool() {
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			registerError("Failed to create command pool!");
			return;
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
			registerError("Failed to allocate command buffers!");
			return;
		}
	}

	void RenderDevice::createSyncObjects() {
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

				registerError("Failed to create synchronization objects for all frames!");
				return;
			}
		}
		for (size_t i = 0; i < swapChain->getImageCount(); i++) {
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
				registerError("Failed to create synchronization objects for all frames!");
				return;
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
			registerError("Failed to begin recording command buffer");
			return;
		}

		if (pipeline == nullptr) {
			registerError("Pipeline not created or stored before recording command buffer!");
			return;
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipeline->getRenderPass();
		renderPassInfo.framebuffer = swapChain->getFramebuffers()[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->getExtent();

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Start of recording
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getGraphicsPipeline());

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

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		// End
		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			registerError("Failed to record command buffer!");
			return;
		}
	}

	void RenderDevice::Draw() {
		ERROR_VOLATILE();
		if (pipeline == nullptr ||
			swapChain == nullptr ||
			commandPool == VK_NULL_HANDLE ||
			imageAvailableSemaphores.size() == 0 ||
			renderFinishedSemaphores.size() == 0 ||
			inFlightFences.size() == 0) {
			registerError("Draw called before Device was fully initilized.");
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
			registerError("Failed to acquire swap chain image!");
			return;
		}

		vkResetFences(device, 1, &inFlightFences[currentFrame]);

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
			registerError("Failed to submit draw command buffer!");
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
		START_WEAK_PTR
			framebufferResized = window->wasFramebufferResized();
		END_WEAK_PTR()

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
			// try again next time
			START_WEAK_PTR
				window->resetFramebufferResizedFlag();
			END_WEAK_PTR()
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS) {
			registerError("Failed to acquire swap chain image!");
			return;
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void RenderDevice::WaitIdle() {
		vkDeviceWaitIdle(device);
	}

	void RenderDevice::recreateSwapChain() {
		START_WEAK_PTR
			// Try again later
			if (window->isWindowMinimized()) { return; }
		END_WEAK_PTR()
		vkDeviceWaitIdle(device);

		SwapChain::SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(physicalDevice, surface);
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		swapChain->constructSwapChain(swapChainSupport, indices, windowReference, surface);
		EXTERN_ERROR(swapChain);
		swapChain->generateFramebuffers(pipeline->getRenderPass());
		EXTERN_ERROR(swapChain);
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