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
	class Vertex {};
	class Vertex2D : public Vertex {
		public:
			glm::vec2 position;
			glm::vec3 color;

			static VkVertexInputBindingDescription getBindingDescriptions();
			static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	};
	template<typename T>
	class VertexBuffer : RenderAsset {
		static_assert(std::is_base_of<Vertex, T>::value, VERTEX_TYPE_MESSAGE);
		public:
			VertexBuffer() = default;
			~VertexBuffer() = default;

			void loadData(const std::vector<T>& verticiesInput);

			const std::string getAssetName() override { return "Vertex Buffer"; }

		private:
			void createVertexBuffer();

			std::vector<T>& verticies{};
			VkBuffer vertexBuffer = VK_NULL_HANDLE;
	};
}
/*

	float: VK_FORMAT_R32_SFLOAT
	vec2: VK_FORMAT_R32G32_SFLOAT
	vec3: VK_FORMAT_R32G32B32_SFLOAT
	vec4: VK_FORMAT_R32G32B32A32_SFLOAT

*/
