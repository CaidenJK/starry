#pragma once

#include "SceneObject.h"

namespace Starry
{
	class MeshObject : public SceneObject {
	public:
		MeshObject(std::string nameInput = "Default");
		~MeshObject();

		void Init() override;
		void Register(Renderer* renderer) override;
		void Destroy() override;

		bool isEmptyMesh() const { return isEmpty; }

		void addVertexData(std::vector<Render::Vertex>& verticesInput, std::vector<uint32_t> indicesInput);
		std::shared_ptr<Render::Buffer>& getRawBuffer() { return Buffer; }

		void loadTextureFromFile(const std::string filePath);
		void loadMeshFromFile(const std::string filePath);

	private:
		bool isEmpty = true;

		std::vector<Render::Vertex> vertices;
		std::vector<uint32_t> indices;

		std::shared_ptr<Render::Buffer> Buffer;
		std::shared_ptr<Render::TextureImage> textureImage;
		//std::shared_ptr<Uniform> Uniform;
	};
}