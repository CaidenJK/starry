#include "SwapChain.h"

#include <vulkan/vk_enum_string_helper.h>

#include <iostream>
#include <algorithm>

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

#define START_WEAK_PTR \
	if (std::shared_ptr<Window> window = windowReference.lock()) {

#define END_WEAK_PTR(x) \
	} else { \
		registerAlert("Window reference is expired!", FATAL); \
		return x; \
	}

namespace StarryRender 
{
	SwapChain::SwapChain(VkDevice& device) : device(device) {
		if (device == VK_NULL_HANDLE) {
			registerAlert("Device is null!", FATAL);
			return;
		}
	}

	SwapChain::~SwapChain() 
	{
		cleanupSwapChain();
	}
	
	void SwapChain::constructSwapChain(SwapChainSupportDetails& swapChainSupport, QueueFamilyIndices& indices, const std::weak_ptr<Window>& windowReference, VkSurfaceKHR& surface) 
	{
		if (device == VK_NULL_HANDLE) {
			registerAlert("Device never set!", FATAL);
			return;
		}
		cleanupSwapChain();
		ERROR_VOLATILE(createSwapChain(swapChainSupport, indices, windowReference, surface));
		createImageViews();
	}

	void SwapChain::createSwapChain(SwapChainSupportDetails& swapChainSupport, QueueFamilyIndices& indices, const std::weak_ptr<Window>& windowReference, VkSurfaceKHR& surface) 
	{
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		ERROR_VOLATILE(VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, windowReference));

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
			registerAlert("Failed to create swap chain!", FATAL);
			return;
		}

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());


		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void SwapChain::createImageViews() 
	{
		swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
				registerAlert("Failed to create image views!", FATAL);
				return;
			}
		}
	}

	void SwapChain::generateFramebuffers(VkRenderPass& renderPass) 
	{
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			VkImageView attachments[] = {
				swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				registerAlert("Failed to create a required framebuffers!", FATAL);
				return;
			}
		}
	}

	void SwapChain::cleanupSwapChain() 
	{
		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}
		swapChainImageViews.clear();

		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}
		swapChainFramebuffers.clear();

		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	SwapChain::SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface) 
	{
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

	VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
	{
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
		registerAlert("Chosen Swap Surface Format: " + std::string(string_VkFormat(currentSwapSurfaceFormat.format)) + ", Color Space: " + std::string(string_VkColorSpaceKHR(currentSwapSurfaceFormat.colorSpace)) + "\n", INFO);
		return currentSwapSurfaceFormat;
	}

	VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
	{
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
		registerAlert("Chosen Present Mode: " + std::string(string_VkPresentModeKHR(currentPresentMode)) + "\n", INFO);
		return currentPresentMode;
	}

	VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const std::weak_ptr<Window>& windowReference) 
	{
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
}