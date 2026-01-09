#include "UniformBuffer.h"

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

#include "Device.h"

namespace StarryRender
{
	UniformBuffer::UniformBuffer()
	{
	}
	
	UniformBuffer::~UniformBuffer()
	{
		destroy();
	}

	void UniformBuffer::init(uint64_t deviceUUID)
	{
		device = requestResource<Device>(deviceUUID, "self");

		createUniformBuffers();
	}

	void UniformBuffer::destroy()
	{
		if (device) {
			for (size_t i = 0; i < uniformBuffers.size(); i++) {
				vkDestroyBuffer((*device).getDevice(), uniformBuffers[i], nullptr);
				vkFreeMemory((*device).getDevice(), uniformBuffersMemory[i], nullptr);
			}
		}
	}

	VkDescriptorBufferInfo UniformBuffer::getDescriptorInfo(int image)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[image];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferData);

		return bufferInfo;
	}

	void UniformBuffer::updateUniformBuffer(uint32_t currentFrame) 
	{
		memcpy(uniformBuffersMapped[currentFrame], &buffer, sizeof(buffer));
	}

	void UniformBuffer::createUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(buffer);

		uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		if (device.wait() != ResourceState::YES) {
			Alert("Device died before it was ready to be used.", FATAL);
			return;
		}
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			(*device).createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory((*device).getDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}
}