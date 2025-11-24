#include "RenderPipeline.h"

#include <fstream>
#include <iostream>

#ifndef NDEBUG
#define SUCCESS_VALIDATION
#endif

#ifdef SUCCESS_VALIDATION

#define THROW_ERROR(msg) \
	error = true; \
	std::cerr << "Pipeline ERROR: " << msg << std::endl; \
	return

#define THROW_ERROR_RETURN(msg, x) \
	error = true; \
	std::cerr << "Pipline ERROR: " << msg << std::endl; \
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
	RenderPipeline::RenderPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath): vertexShaderPath(vertexShaderPath), fragmentShaderPath(fragmentShaderPath) {
		initPipeline();
	}

	RenderPipeline::~RenderPipeline() {

	}

	void RenderPipeline::initPipeline() {
		ERROR_VOLATILE(loadVertexShaderFromFile());
		ERROR_VOLATILE(loadFragmentShaderFromFile());
	}

	void RenderPipeline::loadVertexShaderFromFile() {
		vertexShaderCode = readFile(vertexShaderPath, error);
	}

	void RenderPipeline::loadFragmentShaderFromFile() {
		fragmentShaderCode = readFile(fragmentShaderPath, error);
	}

    std::vector<char> RenderPipeline::readFile(const std::string& filename, bool& error) {
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