#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <array>
#include <type_traits>

#include "Asset.h"

#define VERTEX_TYPE_MESSAGE "VertexBuffer must be created with types: Vertex2D"

namespace StarryRender {
	struct Vertex {
		glm::vec2 position;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescriptions();
		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	};

	class VertexBuffer : RenderAsset {
		public:
			VertexBuffer(VkDevice& device);
			~VertexBuffer();

			void loadData(VkPhysicalDevice physicalDevice, const std::vector<Vertex>& verticiesInput);

			size_t getNumVerticies() { return verticies.size(); }
			VkBuffer& getBuffer() { return vertexBuffer; }

			const std::string getAssetName() override { return "Vertex Buffer"; }

		private:
			void createVertexBuffer(VkPhysicalDevice& physicalDevice);

			void fillBufferData();

			uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

			std::vector<Vertex> verticies;
			VkBuffer vertexBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

			uint32_t bufferSize = 0;

			VkDevice& device;
	};
}
/*

	float: VK_FORMAT_R32_SFLOAT
	vec2: VK_FORMAT_R32G32_SFLOAT
	vec3: VK_FORMAT_R32G32B32_SFLOAT
	vec4: VK_FORMAT_R32G32B32A32_SFLOAT

*/
