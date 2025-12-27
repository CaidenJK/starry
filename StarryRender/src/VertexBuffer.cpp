#include "VertexBuffer.h"

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

namespace StarryRender 
{
	VkVertexInputBindingDescription Vertex::getBindingDescriptions() 
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		// Vert
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		// Color
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

	VertexBuffer::VertexBuffer()
	{
		device = requestResource<VkDevice>("RenderDevice", "VkDevice");
	}

	VertexBuffer::~VertexBuffer() 
	{
		if (device) {
			if (vertexBuffer != VK_NULL_HANDLE) {
				vkDestroyBuffer(*device, vertexBuffer, nullptr);
				vkFreeMemory(*device, vertexBufferMemory, nullptr);
			}

			if (indexBuffer != VK_NULL_HANDLE) {
				vkDestroyBuffer(*device, indexBuffer, nullptr);
				vkFreeMemory(*device, indexBufferMemory, nullptr);
			}

			if (stagingBufferVertex != VK_NULL_HANDLE) {
				vkDestroyBuffer(*device, stagingBufferVertex, nullptr);
			}
			if (stagingBufferMemoryVertex != VK_NULL_HANDLE) {
				vkFreeMemory(*device, stagingBufferMemoryVertex, nullptr);
			}
			if (stagingBufferIndex != VK_NULL_HANDLE) {
				vkDestroyBuffer(*device, stagingBufferIndex, nullptr);
			}
			if (stagingBufferMemoryIndex != VK_NULL_HANDLE) {
				vkFreeMemory(*device, stagingBufferMemoryIndex, nullptr);
			}
		}
	}

	void VertexBuffer::loadData(const std::vector<Vertex>& verticiesInput, const std::vector<uint32_t>& indiciesInput) 
	{
		vertices = verticiesInput;
		indices = indiciesInput;
	}

	void VertexBuffer::createVertexBuffer(VkPhysicalDevice& physicalDevice) 
	{
		if (vertices.empty()) {
			registerAlert("No vertex data loaded into VertexBuffer!", FATAL);
			return;
		}

		bufferSizeVertex = sizeof(vertices[0]) * vertices.size();

		VkCommandBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		ERROR_VOLATILE(createBuffer(physicalDevice, bufferSizeVertex, usageFlags, memoryFlags, stagingBufferVertex, stagingBufferMemoryVertex));
		ERROR_VOLATILE(fillVertexBufferData(stagingBufferMemoryVertex));
		if (vertexBuffer == VK_NULL_HANDLE) {
			VkCommandBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			ERROR_VOLATILE(createBuffer(physicalDevice, bufferSizeVertex, usageFlags, memoryFlags, vertexBuffer, vertexBufferMemory));
		}
	}

	void VertexBuffer::createIndexBuffer(VkPhysicalDevice& physicalDevice) 
	{
		if (indices.empty()) {
			registerAlert("No index data loaded into VertexBuffer!", FATAL);
			return;
		}
		bufferSizeIndex = sizeof(indices[0]) * indices.size();

		VkCommandBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		ERROR_VOLATILE(createBuffer(physicalDevice, bufferSizeIndex, usageFlags, memoryFlags, stagingBufferIndex, stagingBufferMemoryIndex));
		ERROR_VOLATILE(fillIndexBufferData(stagingBufferMemoryIndex));
		if (indexBuffer == VK_NULL_HANDLE) {
			VkCommandBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			ERROR_VOLATILE(createBuffer(physicalDevice, bufferSizeIndex, usageFlags, memoryFlags, indexBuffer, indexBufferMemory));
		}
	}

	void VertexBuffer::loadBufferToMemory(VkPhysicalDevice& physicalDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue)
	{
		createVertexBuffer(physicalDevice);
		createIndexBuffer(physicalDevice);

		if (stagingBufferVertex == VK_NULL_HANDLE ||
			stagingBufferIndex == VK_NULL_HANDLE ||
			stagingBufferMemoryVertex == VK_NULL_HANDLE ||
			stagingBufferMemoryIndex == VK_NULL_HANDLE ||
			vertexBuffer == VK_NULL_HANDLE ||
			indexBuffer == VK_NULL_HANDLE) {
				registerAlert("Load Buffer called before all buffers were created!", CRITICAL);
				return;
		}

		copyBuffer(commandPool, graphicsQueue, stagingBufferVertex, vertexBuffer, bufferSizeVertex);
		copyBuffer(commandPool, graphicsQueue, stagingBufferIndex, indexBuffer, bufferSizeIndex);
		
		device.wait();
		vkDestroyBuffer(*device, stagingBufferVertex, nullptr);
		vkFreeMemory(*device, stagingBufferMemoryVertex, nullptr);
		stagingBufferVertex = VK_NULL_HANDLE;
		stagingBufferMemoryVertex = VK_NULL_HANDLE;

		vkDestroyBuffer(*device, stagingBufferIndex, nullptr);
		vkFreeMemory(*device, stagingBufferMemoryIndex, nullptr);
		stagingBufferIndex = VK_NULL_HANDLE;
		stagingBufferMemoryIndex = VK_NULL_HANDLE;
	}

	void VertexBuffer::fillVertexBufferData(VkDeviceMemory& bufferMemory) 
	{
		if (bufferMemory == VK_NULL_HANDLE) {
			registerAlert("Vertex buffer not created before filling data!", FATAL);
			return;
		}
		if (vertices.empty()) {
			registerAlert("No vertex data loaded into VertexBuffer!", FATAL);
			return;
		}

		void* data;

		device.wait();
		vkMapMemory(*device, bufferMemory, 0, bufferSizeVertex, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSizeVertex);
		vkUnmapMemory(*device, bufferMemory);
	}

	void VertexBuffer::fillIndexBufferData(VkDeviceMemory& bufferMemory) 
	{
		if (bufferMemory == VK_NULL_HANDLE) {
			registerAlert("Vertex buffer not created before filling data!", FATAL);
			return;
		}
		if (indices.empty()) {
			registerAlert("No vertex data loaded into VertexBuffer!", FATAL);
			return;
		}

		void* data;

		device.wait();
		vkMapMemory(*device, bufferMemory, 0, bufferSizeIndex, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSizeIndex);
		vkUnmapMemory(*device, bufferMemory);
	}

	void VertexBuffer::createBuffer(VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
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

	uint32_t VertexBuffer::findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

	void VertexBuffer::copyBuffer(VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;

		device.wait();
		vkAllocateCommandBuffers(*device, &allocInfo, &commandBuffer);

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

		vkFreeCommandBuffers(*device, commandPool, 1, &commandBuffer);
	}
}
// Possibly sendData() with asset handler