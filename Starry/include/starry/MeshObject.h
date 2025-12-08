#pragma once

#include <StarryRender.h>

#include <string>

#include "Asset.h"
#define DEFAULT_NAME_MESH "MeshObject"

namespace Starry
{
	class MeshObject : public StarryAsset {
	public:
		MeshObject(std::string nameInput = DEFAULT_NAME_MESH);
		~MeshObject();

		bool isEmptyMesh() const { return isEmpty; }

		void addVertexData(std::vector<Vertex>& verticesInput, std::vector<uint32_t> indicesInput);
		void registerMeshBuffer(std::unique_ptr<RenderContext>& renderContext);
		void registerMeshBuffer(std::shared_ptr<RenderContext>& renderContext);

		void rotateMesh(float angleRadians, const glm::vec3& axis);

		glm::mat4& getModelMatrix() { return localToGlobalSpace; }
		std::shared_ptr<VertexBuffer>& getRawVertexBuffer() { return vertexBuffer; }

		static MeshObject primitiveCube(float size);
		static MeshObject primitiveQuad(float width, float height);

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