#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Buffer.h"
#include "Descriptor.h"

namespace StarryRender 
{
	struct UniformBufferData {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class UniformBuffer : public Buffer
	{
	public:
		UniformBuffer();
		~UniformBuffer();

		UniformBufferData& getBuffer() { return buffer; }
		void setBuffer(const UniformBufferData& ubo) { buffer = ubo; }

		void updateUniformBuffer(uint32_t currentFrame);

		enum SharedResources 
		{
			VK_BUFFERS = 0
		};

		GET_RESOURCE;
		GET_RESOURCE_FROM_STRING;

		ASSET_NAME("Uniform Buffer")
	private:
		void createUniformBuffers();

		UniformBufferData buffer;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;
	};
}