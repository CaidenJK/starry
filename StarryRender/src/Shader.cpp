#include "Shader.h"

#include <fstream>

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender 
{
	Shader::Shader(VkDevice& device, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) : device(device), vertexShaderPath(vertexShaderPath), fragmentShaderPath(fragmentShaderPath) 
	{
		if (device == VK_NULL_HANDLE) {
			registerAlert("Device is null!", FATAL);
			return;
		}
		initShader();
	}

	Shader::~Shader() 
	{
		if (vertShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(device, vertShaderModule, nullptr);
		}
		if (fragShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(device, fragShaderModule, nullptr);
		}

	}

	void Shader::initShader() 
	{
		ERROR_VOLATILE(loadVertexShaderFromFile());
		ERROR_VOLATILE(loadFragmentShaderFromFile());

		bool error = false;
		vertShaderModule = createShaderModule(device, vertexShaderCode, error);
		if (error) {
			registerAlert("Failed to create vertex shader module!", FATAL);
			return;
		}

		fragShaderModule = createShaderModule(device, fragmentShaderCode, error);
		if (error) {
			registerAlert("Failed to create vertex shader module!", FATAL);
			return;
		}

		ERROR_VOLATILE(bindShaderStages());
	}

	VkShaderModule Shader::createShaderModule(VkDevice& device, const std::vector<char>& code, bool& error) 
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			error = true;
			return {};
		}
		error = false;
		return shaderModule;
	}

	void Shader::bindShaderStages() 
	{
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		shaderStages = { vertShaderStageInfo, fragShaderStageInfo };
	}

	void Shader::loadVertexShaderFromFile() 
	{
		bool error = false;
		vertexShaderCode = readFile(vertexShaderPath, error);
		if (error) {
			registerAlert("Failed to read vertex shader file: " + vertexShaderPath, FATAL);
		}
	}

	void Shader::loadFragmentShaderFromFile() 
	{
		bool error = false;
		fragmentShaderCode = readFile(fragmentShaderPath, error);
		if (error) {
			registerAlert("Failed to read fragment shader file: " + fragmentShaderPath, FATAL);
		}
	}

	std::vector<char> Shader::readFile(const std::string& filename, bool& error) 
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			error = true;
			return {};
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		error = false;
		return buffer;
	}
}