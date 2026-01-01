#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <memory>

#include <StarryManager.h>

#include "VertexBuffer.h"
#include "Shader.h"
#include "Descriptor.h"

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

		ASSET_NAME("Pipeline")

	private:
		void constructPipeline(std::array<VkFormat, 2>& imageFormats, std::shared_ptr<Descriptor>& descriptor);
		void createRenderPass(std::array<VkFormat, 2>& imageFormats);
		void constructPipelineLayout(std::shared_ptr<Descriptor>& descriptor);

		VkPipelineVertexInputStateCreateInfo createVertexInputInfo();

		std::shared_ptr<Shader> shader = nullptr;

		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline graphicsPipeline = VK_NULL_HANDLE;

		ResourceHandle<VkDevice> device{};
	};
}