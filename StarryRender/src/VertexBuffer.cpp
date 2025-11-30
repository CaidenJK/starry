#include "VertexBuffer.h"

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender {
	VkVertexInputBindingDescription Vertex::getBindingDescriptions() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		// Vert
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		// Color
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

	VertexBuffer::VertexBuffer(VkDevice& deviceRef) : device(deviceRef) {
		if (device == VK_NULL_HANDLE) {
			registerAlert("Null Vulkan device provided to VertexBuffer!", FATAL);
		}
	}
	VertexBuffer::~VertexBuffer() {
		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, vertexBufferMemory, nullptr);

		if (stagingBuffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(device, stagingBuffer, nullptr);
		}
		if (stagingBufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(device, stagingBufferMemory, nullptr);
		}
	}

	void VertexBuffer::loadData(VkPhysicalDevice physicalDevice, const std::vector<Vertex>& verticiesInput) {
		verticies = verticiesInput;
		createVertexBuffer(physicalDevice);
	}

	void VertexBuffer::createVertexBuffer(VkPhysicalDevice& physicalDevice) {
		VkDeviceSize bufferSize = sizeof(verticies[0]) * verticies.size();
		ERROR_VOLATILE(createBuffer(physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory));
		ERROR_VOLATILE(fillBufferData(stagingBufferMemory));
		if (vertexBuffer == VK_NULL_HANDLE) {
			ERROR_VOLATILE(createBuffer(physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory));
		}
	}

	void VertexBuffer::loadBufferToMemory(VkCommandPool& commandPool, VkQueue& graphicsQueue) {
		if (stagingBuffer == VK_NULL_HANDLE || vertexBuffer == VK_NULL_HANDLE) {
			registerAlert("Load Buffer called before buffers were created!", CRITICAL);
			return;
		}
		copyBuffer(commandPool, graphicsQueue, stagingBuffer, vertexBuffer, bufferSize);
		
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		stagingBuffer = VK_NULL_HANDLE;
		stagingBufferMemory = VK_NULL_HANDLE;
	}

	void VertexBuffer::createBuffer(VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		if (verticies.empty()) {
			registerAlert("No vertex data loaded into VertexBuffer!", FATAL);
			return;
		}
		if (vertexBuffer != VK_NULL_HANDLE) {
			registerAlert("Vertex buffer already created! All calls other than the first are skipped.", WARNING);
			return;
		}

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferSize = bufferInfo.size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			registerAlert("Failed to create buffer!", FATAL);
			return;
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			registerAlert("Failed to allocate buffer memory!", FATAL);
			return;
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void VertexBuffer::fillBufferData(VkDeviceMemory& bufferMemory) {
		if (bufferMemory == VK_NULL_HANDLE) {
			registerAlert("Vertex buffer not created before filling data!", FATAL);
			return;
		}
		if (verticies.empty()) {
			registerAlert("No vertex data loaded into VertexBuffer!", FATAL);
			return;
		}

		void* data;
		vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, verticies.data(), (size_t)bufferSize);
		vkUnmapMemory(device, bufferMemory);
	}

	void VertexBuffer::copyBuffer(VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size) {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	uint32_t VertexBuffer::findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
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
// Possibly sendData() with asset handler