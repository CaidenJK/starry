#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <array>

#include "Asset.h"

namespace StarryRender 
{
	class Shader : public RenderAsset {
		public:
			Shader(VkDevice& device, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
			~Shader();

			std::array<VkPipelineShaderStageCreateInfo, 2>& getShaderStages() { return shaderStages; }

			const std::string getAssetName() override { return "Shader"; }

		private:
			static VkShaderModule createShaderModule(VkDevice& device, const std::vector<char>& code, bool& error);

			static std::vector<char> readFile(const std::string& filename, bool& error);

			void initShader();

			void bindShaderStages();

			void loadVertexShaderFromFile();
			void loadFragmentShaderFromFile();

			const std::string vertexShaderPath;
			const std::string fragmentShaderPath;

			std::vector<char> vertexShaderCode = {};
			std::vector<char> fragmentShaderCode = {};

			VkShaderModule vertShaderModule = VK_NULL_HANDLE;
			VkShaderModule fragShaderModule = VK_NULL_HANDLE;

			std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {};

			VkDevice& device;
	};
}