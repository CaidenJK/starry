#include "UniformBuffer.h"

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender
{
	UniformBuffer::UniformBuffer()
	{
		createUniformBuffers();
	}
	
	UniformBuffer::~UniformBuffer()
	{
		if (device) {
			for (size_t i = 0; i < uniformBuffers.size(); i++) {
				vkDestroyBuffer(*device, uniformBuffers[i], nullptr);
				vkFreeMemory(*device, uniformBuffersMemory[i], nullptr);
			}
		}
	}

	std::optional<void*> UniformBuffer::getResource(size_t resourceID)
	{
		if (resourceID == SharedResources::VK_BUFFERS) {
			return (void*)&uniformBuffers;
		}

		registerAlert(std::string("No matching resource: ") + std::to_string(resourceID) + " available for sharing", WARNING);
		return {};
	}

	size_t UniformBuffer::getResourceIDFromString(std::string resourceName)
	{
		if (resourceName.compare("VkBuffers") == 0) {
			return SharedResources::VK_BUFFERS;
		}
		return INVALID_RESOURCE;
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
			registerAlert("Device died before it was ready to be used.", FATAL);
			return;
		}
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory(*device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}
}