#include "UniformBuffer.h"

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender
{
	UniformBuffer::UniformBuffer()
	{
		createDescriptorSetLayout(); ERROR_VOLATILE();
		createDescriptorPool();
	}
	UniformBuffer::~UniformBuffer()
	{
		if (device) {
			for (size_t i = 0; i < uniformBuffers.size(); i++) {
				vkDestroyBuffer(*device, uniformBuffers[i], nullptr);
				vkFreeMemory(*device, uniformBuffersMemory[i], nullptr);
			}

			vkDestroyDescriptorPool(*device, descriptorPool, nullptr);

			vkDestroyDescriptorSetLayout(*device, descriptorSetLayout, nullptr);
		}
	}

	void UniformBuffer::attachBuffer()
	{
		createUniformBuffers(); ERROR_VOLATILE();
		createDescriptorSets();
	}

	void UniformBuffer::updateUniformBuffer(uint32_t currentFrame) 
	{
		memcpy(uniformBuffersMapped[currentFrame], &buffer, sizeof(buffer));
	}

	void UniformBuffer::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;
		
		if (device.wait() != ResourceState::YES) {
			registerAlert("Device died before it was ready to be used.", FATAL);
			return;
		}
		if (vkCreateDescriptorSetLayout(*device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			registerAlert("Failed to create descriptor set layout!", FATAL);
		}
	}

	void UniformBuffer::createUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(buffer);

		uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		if (device.wait() != ResourceState::YES) {
			registerAlert("Device died before it was ready to be used.", FATAL);
			return;
		}
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory(*device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void UniformBuffer::createDescriptorPool()
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;

		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		if (device.wait() != ResourceState::YES) {
			registerAlert("Device died before it was ready to be used.", FATAL);
			return;
		}
		if (vkCreateDescriptorPool(*device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			registerAlert("Failed to create descriptor pool!", FATAL);
			return;
		}

	}

	void UniformBuffer::createDescriptorSets()
	{
		if (descriptorPool == VK_NULL_HANDLE || descriptorSetLayout == VK_NULL_HANDLE) {
			registerAlert("Descriptor pool or set layout not ready before allocating descriptor sets.", FATAL);
			return;
		}

		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

		if (device.wait() != ResourceState::YES) {
			registerAlert("Device died before it was ready to be used.", FATAL);
			return;
		}
		if (vkAllocateDescriptorSets(*device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			registerAlert("Failed to allocate descriptor sets!", FATAL);
			return;
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(buffer);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(*device, 1, &descriptorWrite, 0, nullptr);
		}
	}
}