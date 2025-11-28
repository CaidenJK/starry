#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <array>

namespace StarryRender {
	class RenderPipeline {
	public:
		RenderPipeline(VkDevice& device, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		~RenderPipeline();

		RenderPipeline operator=(const RenderPipeline&) = delete;
		RenderPipeline(const RenderPipeline&) = delete;

		bool getError() { return error; }
		void loadShadersFromFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

		void constructPipeline(VkFormat swapChainImageFormat);

		VkRenderPass& getRenderPass() { return renderPass; }

		VkPipeline& getGraphicsPipeline() { return graphicsPipeline; }

	private:
		void initPipeline();
		void bindShaderStages();
		void createRenderPass(VkFormat swapChainImageFormat);
		void constructPipelineLayout();

		static VkShaderModule createShaderModule(VkDevice& device, const std::vector<char>& code, bool& error);

		static std::vector<char> readFile(const std::string& filename, bool& error);
		void loadVertexShaderFromFile();
		void loadFragmentShaderFromFile();

		const std::string& vertexShaderPath;
		const std::string& fragmentShaderPath;

		std::vector<char> vertexShaderCode = {};
		std::vector<char> fragmentShaderCode = {};

		VkShaderModule vertShaderModule = VK_NULL_HANDLE;
		VkShaderModule fragShaderModule = VK_NULL_HANDLE;

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {};

		VkViewport viewport{};
		VkRect2D scissor{};

		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline = VK_NULL_HANDLE;

		VkDevice device = VK_NULL_HANDLE;

		bool error = false;
	};
}