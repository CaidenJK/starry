#include "Shader.h"

#include <iostream>
#include <fstream>

#ifndef NDEBUG
#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION

#define THROW_ERROR(msg) \
	error = true; \
	std::cerr << "Shader ERROR: " << msg << std::endl; \
	return

#define THROW_ERROR_RETURN(msg, x) \
	error = true; \
	std::cerr << "Shader ERROR: " << msg << std::endl; \
	return x


#define ALERT_MSG(msg) \
	std::cout << msg

#else
#define THROW_ERROR(msg) \
	error = true; \
	return

#define ALERT_MSG(msg)

#define THROW_ERROR_RETURN(msg, x) \
	error = true; \
	return x

#endif

#define ERROR_VOLATILE(x) x; if (error) { return; }

namespace StarryRender {
	Shader::Shader(VkDevice& device, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) : device(device), vertexShaderPath(vertexShaderPath), fragmentShaderPath(fragmentShaderPath) {
		if (device == VK_NULL_HANDLE) {
			THROW_ERROR("Device is null!");
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
		ERROR_VOLATILE(vertShaderModule = createShaderModule(device, vertexShaderCode, error));
		ERROR_VOLATILE(fragShaderModule = createShaderModule(device, fragmentShaderCode, error));
		ERROR_VOLATILE(bindShaderStages());
	}

	VkShaderModule Shader::createShaderModule(VkDevice& device, const std::vector<char>& code, bool& error) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			THROW_ERROR_RETURN("Failed to create shader module.", {});
		}
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
		vertexShaderCode = readFile(vertexShaderPath, error);
	}

	void Shader::loadFragmentShaderFromFile() {
		fragmentShaderCode = readFile(fragmentShaderPath, error);
	}

	std::vector<char> Shader::readFile(const std::string& filename, bool& error) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			THROW_ERROR_RETURN("Failed to open file: " << filename, {});
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
}