#include "Shader.h"

#include <iostream>
#include <fstream>

#ifndef NDEBUG
#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION

#define ALERT_MSG(msg) \
	std::cout << msg

#else

#define ALERT_MSG(msg)

#endif

#define ERROR_VOLATILE(x) x; if (getError()) { return; }

namespace StarryRender {
	Shader::Shader(VkDevice& device, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) : device(device), vertexShaderPath(vertexShaderPath), fragmentShaderPath(fragmentShaderPath) {
		if (device == VK_NULL_HANDLE) {
			registerError("Device is null!");
			return;
		}
		initShader();
	}

	Shader::~Shader() {
		if (vertShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(device, vertShaderModule, nullptr);
		}
		if (fragShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(device, fragShaderModule, nullptr);
		}

	}

	void Shader::initShader() {
		ERROR_VOLATILE(loadVertexShaderFromFile());
		ERROR_VOLATILE(loadFragmentShaderFromFile());

		bool error = false;
		vertShaderModule = createShaderModule(device, vertexShaderCode, error);
		if (error) {
			registerError("Failed to create vertex shader module!");
			return;
		}

		fragShaderModule = createShaderModule(device, fragmentShaderCode, error);
		if (error) {
			registerError("Failed to create vertex shader module!");
			return;
		}

		ERROR_VOLATILE(bindShaderStages());
	}

	VkShaderModule Shader::createShaderModule(VkDevice& device, const std::vector<char>& code, bool& error) {
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

	void Shader::bindShaderStages() {
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

	void Shader::loadVertexShaderFromFile() {
		bool error = false;
		vertexShaderCode = readFile(vertexShaderPath, error);
		if (error) {
			registerError("Failed to read vertex shader file: " + vertexShaderPath);
		}
	}

	void Shader::loadFragmentShaderFromFile() {
		bool error = false;
		fragmentShaderCode = readFile(fragmentShaderPath, error);
		if (error) {
			registerError("Failed to read fragment shader file: " + fragmentShaderPath);
		}
	}

	std::vector<char> Shader::readFile(const std::string& filename, bool& error) {
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