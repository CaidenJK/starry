#include "MeshObject.h"

#include <glm/gtc/matrix_transform.hpp>

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; }

namespace Starry
{
	MeshObject::MeshObject(std::string nameInput) :  SceneObject(std::string("Mesh, ") + nameInput)
	{

	}

	MeshObject::~MeshObject() 
	{
		Destroy();
	}

	void MeshObject::Destroy()
	{
		if (vertexBuffer != nullptr) {
			vertexBuffer.reset();
		}
		if (textureImage != nullptr) {
			textureImage.reset();
		}
	}

	void MeshObject::addVertexData(std::vector<Vertex>& verticesInput, std::vector<uint32_t> indicesInput) 
	{
		vertices = verticesInput;
		indices = indicesInput;
		isEmpty = vertices.empty() || indices.empty();
	}

	void MeshObject::Register(Renderer* renderer)
	{
		if (isEmptyMesh()) {
			registerAlert("Cannot register empty mesh buffer!", FATAL);
			return;
		}
		vertexBuffer = std::make_shared<VertexBuffer>();

		vertexBuffer->loadData(vertices, indices);
		renderer->context().loadVertexBuffer(vertexBuffer);

		textureImage->loadFromFile();
		renderer->context().loadTextureImage(textureImage);
	}

	void MeshObject::loadTextureFromFile(const std::string filePath)
	{
		textureImage = std::make_shared<TextureImage>();
		textureImage->storeFilePath(filePath);
	}

	void MeshObject::loadMeshFromFile(const std::string filePath)
	{
		auto file = requestResource<FILETYPE>(FILE_REQUEST, filePath, {Flags::READ | Flags::MODEL});

		if (file.wait() != ResourceState::YES) {
			registerAlert("Could not open mesh file.", CRITICAL);
			return;
		}

		auto meshFile = dynamic_cast<ModelFile*>(*file);

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : meshFile->shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};
				vertex.position = {
					meshFile->attrib.vertices[3 * index.vertex_index + 0],
					meshFile->attrib.vertices[3 * index.vertex_index + 1],
					meshFile->attrib.vertices[3 * index.vertex_index + 2]
				};
				vertex.texCoord = {
					meshFile->attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - meshFile->attrib.texcoords[2 * index.texcoord_index + 1]
				};
				vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}

		addVertexData(vertices, indices);
	}
}