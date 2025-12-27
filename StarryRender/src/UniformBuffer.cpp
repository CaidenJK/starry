#include "UniformBuffer.h"

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender
{
	UniformBuffer::UniformBuffer()
	{
		device = requestResource<VkDevice>("RenderDevice", "VkDevice");

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

	void UniformBuffer::attachBuffer(VkPhysicalDevice& physicalDevice)
	{
		createUniformBuffers(physicalDevice); ERROR_VOLATILE();
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
		
		device.wait();
		if (vkCreateDescriptorSetLayout(*device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			registerAlert("Failed to create descriptor set layout!", FATAL);
		}
	}

	void UniformBuffer::createUniformBuffers(VkPhysicalDevice& physicalDevice)
	{
		VkDeviceSize bufferSize = sizeof(buffer);

		uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		device.wait();
		VkCommandBufferUsageFlags usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			createBuffer(physicalDevice, bufferSize, usageFlags, memoryFlags, uniformBuffers[i], uniformBuffersMemory[i]);

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

		device.wait();
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

		device.wait();
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

	void UniformBuffer::createBuffer(VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		if (buffer != VK_NULL_HANDLE || bufferMemory != VK_NULL_HANDLE) {
			registerAlert("Vertex buffer already created! All calls other than the first are skipped.", WARNING);
			return;
		}

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		device.wait();
		if (vkCreateBuffer(*device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			registerAlert("Failed to create buffer!", FATAL);
			return;
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(*device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(*device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			registerAlert("Failed to allocate buffer memory!", FATAL);
			return;
		}

		vkBindBufferMemory(*device, buffer, bufferMemory, 0);
	}

	uint32_t UniformBuffer::findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		if (physicalDevice == VK_NULL_HANDLE) {
			registerAlert("Vulkan physical device null! Can't find memory type.", FATAL);
			return 0;
		}
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		registerAlert("Failed to find suitable memory type on given device!", FATAL);
		return 0;
	}
}