#include "VertexBuffer.h"

namespace StarryRender {
	VkVertexInputBindingDescription Vertex2D::getBindingDescriptions() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex2D);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	std::array<VkVertexInputAttributeDescription, 2> Vertex2D::getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		// Vert
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex2D, position);

		// Color
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex2D, color);

		return attributeDescriptions;
	}

	template<typename T>
	void VertexBuffer<T>::loadData(const std::vector<T>& verticiesInput) {
		verticies = verticiesInput;
		createVertexBuffer();
	}

	template<typename T>
	void VertexBuffer<T>::createVertexBuffer() {
		if (verticies.empty()) {
			registerAlert("No vertex data loaded into VertexBuffer!", FATAL);
			return;
		}
		if (vertexBuffer != VK_NULL_HANDLE) {
			registerAlert("Vertex buffer already created! All calls other than the first are skipped.", WARNING);
			return;
		}
	}
}