#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <array>
#include <optional>

namespace StarryRender {
	class RenderPipeline {
	public:
		RenderPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		RenderPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, VkDevice& deviceRef);
		~RenderPipeline();

		bool getError() { return error; }
		void loadShadersFromFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

		void constructPipeline(VkDevice& deviceRef);

	private:
		void initPipeline();
		void bindShaderStages();
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

		VkDevice device;

		bool error = false;
	};
}