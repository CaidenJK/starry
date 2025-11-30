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
	}

	void VertexBuffer::loadData(VkPhysicalDevice physicalDevice, const std::vector<Vertex>& verticiesInput) {
		verticies = verticiesInput;
		ERROR_VOLATILE(createVertexBuffer(physicalDevice));
		ERROR_VOLATILE(fillBufferData());
	}

	void VertexBuffer::createVertexBuffer(VkPhysicalDevice& physicalDevice) {
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
		bufferInfo.size = sizeof(verticies[0]) * verticies.size();
		bufferSize = bufferInfo.size;

		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
			registerAlert("Failed to create VertexBuffer!", FATAL);
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
			registerAlert("Failed to allocate vertex buffer memory!", FATAL);
			return;
		}
		vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
	}

	void VertexBuffer::fillBufferData() {
		if (vertexBuffer == VK_NULL_HANDLE) {
			registerAlert("Vertex buffer not created before filling data!", FATAL);
			return;
		}
		if (verticies.empty()) {
			registerAlert("No vertex data loaded into VertexBuffer!", FATAL);
			return;
		}

		void* data;
		vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, verticies.data(), (size_t)bufferSize);
		vkUnmapMemory(device, vertexBufferMemory);

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