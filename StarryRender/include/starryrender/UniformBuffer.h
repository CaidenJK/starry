#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include "glm/glm.hpp"

#include <StarryAsset.h>
#include "Buffer.h"

#define MAX_FRAMES_IN_FLIGHT 2

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

		void attatchBuffer(VkPhysicalDevice& physicalDevice); // TODO: fix this whole mess

		UniformBufferData& getBuffer() { return buffer; }
		void setBuffer(const UniformBufferData& ubo) { buffer = ubo; }

		void updateUniformBuffer(uint32_t currentFrame);

		VkDescriptorSetLayout& getDescriptorSetLayout() { return descriptorSetLayout; }
		VkDescriptorSet& getDescriptorSet(uint32_t index) { return descriptorSets[index]; }

		const std::string getAssetName() override { return "Uniform Buffer"; }
	private:
		void createDescriptorSetLayout();
		void createUniformBuffers(VkPhysicalDevice& physicalDevice);	
		void createDescriptorPool();
		void createDescriptorSets();

		UniformBufferData buffer;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> descriptorSets;
	};
}