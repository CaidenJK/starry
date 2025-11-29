#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <memory>

#include <memory>

#include "Asset.h"
#include "Shader.h"

namespace StarryRender {
	class RenderPipeline : public RenderAsset {
	public:
		RenderPipeline(VkDevice& device);
		~RenderPipeline();

		RenderPipeline operator=(const RenderPipeline&) = delete;
		RenderPipeline(const RenderPipeline&) = delete;

		void loadShader(std::shared_ptr<Shader>& shaderValue);

		void constructPipeline(VkFormat swapChainImageFormat);

		VkRenderPass& getRenderPass() { return renderPass; }

		VkPipeline& getGraphicsPipeline() { return graphicsPipeline; }

	private:
		void createRenderPass(VkFormat swapChainImageFormat);
		void constructPipelineLayout();

		std::shared_ptr<Shader> shader = nullptr;

		//VkViewport viewport{};
		//VkRect2D scissor{};

		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline graphicsPipeline = VK_NULL_HANDLE;

		VkDevice& device;
	};
}