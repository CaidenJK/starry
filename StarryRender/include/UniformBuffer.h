#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <StarryManager.h>

#include "glm/glm.hpp"

namespace StarryRender 
{
	struct UniformBufferData {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class Device;

	class UniformBuffer : public StarryAsset
	{
	public:
		UniformBuffer();
		~UniformBuffer();

		void init(uint64_t deviceUUID);
		void destroy();

		UniformBufferData& getBuffer() { return buffer; }
		void setData(const UniformBufferData& ubo) { buffer = ubo; }

		VkDescriptorBufferInfo getDescriptorInfo(int image);

		void updateUniformBuffer(uint32_t currentFrame);

		ASSET_NAME("Uniform Buffer")
	private:
		void createUniformBuffers();

		UniformBufferData buffer;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

		ResourceHandle<Device> device;
	};
}