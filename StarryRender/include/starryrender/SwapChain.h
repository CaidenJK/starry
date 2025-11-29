#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <optional>

#include "Asset.h"
#include "Window.h"

namespace StarryRender {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	class SwapChain : public RenderAsset {
		public:
			struct SwapChainSupportDetails {
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector<VkSurfaceFormatKHR> formats;
				std::vector<VkPresentModeKHR> presentModes;
			};
		public:
			SwapChain(VkDevice& device);
			~SwapChain();

			SwapChain operator=(const SwapChain&) = delete;
			SwapChain(const SwapChain&) = delete;

			void constructSwapChain(SwapChainSupportDetails& swapChainSupport, QueueFamilyIndices& indices, const std::weak_ptr<Window>& windowReference, VkSurfaceKHR& surface);
			void generateFramebuffers(VkRenderPass& renderPass);

			std::vector<VkFramebuffer>& getFramebuffers() { return swapChainFramebuffers; }
			VkSwapchainKHR getSwapChain() { return swapChain; }


			VkFormat getImageFormat() { return swapChainImageFormat; }
			VkExtent2D getExtent() { return swapChainExtent; }
			size_t getImageCount() { return swapChainImages.size(); }

			static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface);
		private:
			void createSwapChain(SwapChainSupportDetails& swapChainSupport, QueueFamilyIndices& indices, const std::weak_ptr<Window>& windowReference, VkSurfaceKHR& surface);
			void createImageViews();
			void cleanupSwapChain();

			VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const std::weak_ptr<Window>& windowReference);

			VkSwapchainKHR swapChain = VK_NULL_HANDLE;

			std::vector<VkImage> swapChainImages;
			VkFormat swapChainImageFormat;
			VkExtent2D swapChainExtent;

			std::vector<VkImageView> swapChainImageViews;

			std::vector<VkFramebuffer> swapChainFramebuffers;

			VkDevice& device;
	};
}