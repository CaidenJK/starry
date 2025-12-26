#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <memory>

#include <StarryAsset.h>

#include "VertexBuffer.h"
#include "Shader.h"
#include "UniformBuffer.h"

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender 
{
	class RenderPipeline : public StarryAsset {
	public:
		RenderPipeline(std::shared_ptr<Shader>& shaderValue);
		~RenderPipeline();

		RenderPipeline operator=(const RenderPipeline&) = delete;
		RenderPipeline(const RenderPipeline&) = delete;

		VkRenderPass& getRenderPass() { return renderPass; }

		VkPipeline& getGraphicsPipeline() { return graphicsPipeline; }

		VkPipelineLayout& getPipelineLayout() { return pipelineLayout; }

		const std::string getAssetName() override { return "Pipeline"; }

	private:
		void constructPipeline(ResourceHandle<VkFormat>& swapChainImageFormat, ResourceHandle<std::weak_ptr<UniformBuffer>>& uniformBuffer);
		void createRenderPass(VkFormat swapChainImageFormat);
		void constructPipelineLayout(std::weak_ptr<UniformBuffer>& uniformBuffer);

		VkPipelineVertexInputStateCreateInfo createVertexInputInfo();

		std::shared_ptr<Shader> shader = nullptr;

		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline graphicsPipeline = VK_NULL_HANDLE;

		ResourceHandle<VkDevice> device;
	};
}