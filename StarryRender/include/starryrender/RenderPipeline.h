#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

namespace StarryRender {
	class RenderPipeline {
	public:
		RenderPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		~RenderPipeline();

		bool getError() { return error; }

	private:
		void initPipeline();

		static std::vector<char> readFile(const std::string& filename, bool& error);
		void loadVertexShaderFromFile();
		void loadFragmentShaderFromFile();

		const std::string& vertexShaderPath;
		const std::string& fragmentShaderPath;

		std::vector<char> vertexShaderCode;
		std::vector<char> fragmentShaderCode;

		bool error = false;
	};
}