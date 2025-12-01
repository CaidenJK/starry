#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include "glm/glm.hpp"

#include "Asset.h"

namespace StarryRender 
{
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class UniformBuffer : public RenderAsset {
	public:
		UniformBuffer(VkDevice& deviceRef);
		~UniformBuffer();

		void attatchBuffer(VkPhysicalDevice& physicalDevice);

		void setModelMatrix(const glm::mat4& modelMatrix) { buffer.model = modelMatrix; }
		void setViewMatrix(const glm::mat4& viewMatrix) { buffer.view = viewMatrix; }
		void setProjectionMatrix(const glm::mat4& projectionMatrix) { buffer.proj = projectionMatrix; }

		void setMVP(UniformBufferObject& ubo) { buffer = ubo; }
		void setMVP(glm::mat4& model, glm::mat4& view, glm::mat4& proj) { buffer = { model, view, proj }; }

		void updateUniformBuffer(uint32_t currentFrame);

		VkDescriptorSetLayout& getDescriptorSetLayout() { return descriptorSetLayout; }
		VkDescriptorSet& getDescriptorSet(uint32_t index) { return descriptorSets[index]; }

		const std::string getAssetName() override { return "UniformBuffer"; }
	private:
		void createDescriptorSetLayout();
		void createUniformBuffers(VkPhysicalDevice& physicalDevice);	
		void createDescriptorPool();
		void createDescriptorSets();

		void createBuffer(VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		UniformBufferObject buffer;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> descriptorSets;

		VkDevice& device;
	};
}