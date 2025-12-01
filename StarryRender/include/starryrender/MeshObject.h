#pragma once

#include <string>

#include "Asset.h"
#include "VertexBuffer.h"

#define DEFAULT_NAME_MESH "MeshObject"

namespace StarryRender 
{
	class MeshObject : public RenderAsset {
	public:
		MeshObject(std::string nameInput = DEFAULT_NAME_MESH);
		~MeshObject();

		bool isEmptyMesh() const { return isEmpty; }

		void addVertexData(std::vector<Vertex>& verticesInput, std::vector<uint32_t> indicesInput);
		void attatchBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice);

		void rotateMesh(float angleRadians, const glm::vec3& axis);

		glm::mat4& getModelMatrix() { return localToGlobalSpace; }

		std::shared_ptr<VertexBuffer>& getRawVertexBuffer() { return vertexBuffer; }
		const std::string getAssetName() override { return const_cast<std::string&>(name); }
	private:
		std::string name;

		bool isEmpty = true;

		glm::mat4 localToGlobalSpace = 1.0f;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::shared_ptr<VertexBuffer> vertexBuffer;
	};
}