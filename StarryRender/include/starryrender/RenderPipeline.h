#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <memory>

#include "Asset.h"
#include "VertexBuffer.h"
#include "Shader.h"

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender {
	class RenderPipeline : public RenderAsset {
	public:
		RenderPipeline(VkDevice& device);
		~RenderPipeline();

		RenderPipeline operator=(const RenderPipeline&) = delete;
		RenderPipeline(const RenderPipeline&) = delete;

		void loadShader(std::shared_ptr<Shader>& shaderValue);

		template<typename T>
		void constructPipeline(VkFormat swapChainImageFormat) {
			static_assert(std::is_base_of<Vertex, T>::value, VERTEX_TYPE_MESSAGE);

			if (graphicsPipeline != VK_NULL_HANDLE || getAlertSeverity() == FATAL) {
				registerAlert("Warning: constructPipeline called more than once. All calls other than the first are skipped.", WARNING);
				return;
			}

			if (shader == nullptr) {
				registerAlert("Shader not loaded before pipeline construction!", FATAL);
				return;
			}

			ERROR_VOLATILE(createRenderPass(swapChainImageFormat));
			ERROR_VOLATILE(constructPipelineLayout(createVertexInputInfo<T>()));

			registerAlert("Successful Pipeline Creation!", INFO);
		}

		VkRenderPass& getRenderPass() { return renderPass; }

		VkPipeline& getGraphicsPipeline() { return graphicsPipeline; }

		const std::string getAssetName() override { return "Pipeline"; }

	private:
		void createRenderPass(VkFormat swapChainImageFormat);
		void constructPipelineLayout(VkPipelineVertexInputStateCreateInfo vertexInputInfo);

		template<typename T>
		VkPipelineVertexInputStateCreateInfo createVertexInputInfo() {
			auto bindingDescription = T::getBindingDescriptions();
			auto attributeDescriptions = T::getAttributeDescriptions();

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			return vertexInputInfo;
		}

		std::shared_ptr<Shader> shader = nullptr;

		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline graphicsPipeline = VK_NULL_HANDLE;

		VkDevice& device;
	};
}