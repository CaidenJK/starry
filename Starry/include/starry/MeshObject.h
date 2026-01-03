#pragma once

#include "SceneObject.h"

namespace Starry
{
	class MeshObject : public SceneObject {
	public:
		MeshObject(std::string nameInput = "Default");
		~MeshObject();

		void Init() override {}
		void Register(Renderer* renderer) override;
		void Destroy() override;

		bool isEmptyMesh() const { return isEmpty; }

		void addVertexData(std::vector<Vertex>& verticesInput, std::vector<uint32_t> indicesInput);
		std::shared_ptr<VertexBuffer>& getRawVertexBuffer() { return vertexBuffer; }

		void loadTextureFromFile(const std::string filePath);
		void loadMeshFromFile(const std::string filePath);

	private:
		bool isEmpty = true;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<TextureImage> textureImage;
	};
}