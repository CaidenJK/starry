#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <array>
#include <type_traits>

#include "Asset.h"

// Helpful debug colors
#define RED_COLOR glm::vec3(1.0f, 0.0f, 0.0f)
#define GREEN_COLOR glm::vec3(0.0f, 1.0f, 0.0f)
#define BLUE_COLOR glm::vec3(0.0f, 0.0f, 1.0f)
#define CYAN_COLOR glm::vec3(0.0f, 1.0f, 1.0f)
#define MAGENTA_COLOR glm::vec3(1.0f, 0.0f, 1.0f)
#define YELLOW_COLOR glm::vec3(1.0f, 1.0f, 0.0f)
#define WHITE_COLOR glm::vec3(1.0f, 1.0f, 1.0f)
#define BLACK_COLOR glm::vec3(0.1f, 0.1f, 0.1f)

namespace StarryRender
{
	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescriptions();
		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	};

	class VertexBuffer : public RenderAsset {
		public:
			VertexBuffer(VkDevice& device);
			~VertexBuffer();

			void loadData(VkPhysicalDevice physicalDevice, const std::vector<Vertex>& verticiesInput, const std::vector<uint32_t>& indiciesInput);
			void loadBufferToMemory(VkCommandPool& commandPool, VkQueue& graphicsQueue);

			size_t getNumVerticies() { return vertices.empty() ? 0 : vertices.size(); }
			size_t getNumIndicies() { return indices.empty() ? 0 : indices.size(); }
			VkBuffer& getVertexBuffer() { return vertexBuffer; }
			VkBuffer& getIndexBuffer() { return indexBuffer; }

			const std::string getAssetName() override { return "Vertex Buffer"; }

		private:
			void createVertexBuffer(VkPhysicalDevice& physicalDevice);
			void createIndexBuffer(VkPhysicalDevice& physicalDevice);

			void createBuffer(VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
			void fillVertexBufferData(VkDeviceMemory& bufferMemory);
			void fillIndexBufferData(VkDeviceMemory& bufferMemory);
			void copyBuffer(VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);

			uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

			std::vector<Vertex> vertices = {};
			std::vector<uint32_t> indices = {};

			VkBuffer stagingBufferVertex = VK_NULL_HANDLE;
			VkDeviceMemory stagingBufferMemoryVertex = VK_NULL_HANDLE;
			VkBuffer stagingBufferIndex = VK_NULL_HANDLE;
			VkDeviceMemory stagingBufferMemoryIndex = VK_NULL_HANDLE;

			VkBuffer vertexBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

			VkBuffer indexBuffer = VK_NULL_HANDLE;
			VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

			VkDeviceSize bufferSizeVertex = 0;
			VkDeviceSize bufferSizeIndex = 0;

			VkDevice& device;
	};
}
/*

	float: VK_FORMAT_R32_SFLOAT
	vec2: VK_FORMAT_R32G32_SFLOAT
	vec3: VK_FORMAT_R32G32B32_SFLOAT
	vec4: VK_FORMAT_R32G32B32A32_SFLOAT

*/
