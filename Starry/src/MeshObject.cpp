#include "MeshObject.h"

#include <glm/gtc/matrix_transform.hpp>

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; }

namespace Starry
{
	MeshObject::MeshObject(std::string nameInput) :  SceneObject(SceneObject::Type::MESH, std::string("Mesh, ") + nameInput)
	{
		buffer = std::make_shared<Render::Buffer>();
		uniform = std::make_shared<Render::Uniform>();
		textureImage = std::make_shared<Render::TextureImage>();
		descriptorSet = std::make_shared<Render::DescriptorSet>();
	}

	MeshObject::~MeshObject() 
	{
		Destroy();
	}

	void MeshObject::Init()
	{
		
	}

	void MeshObject::Destroy()
	{
		if (buffer != nullptr) {
			buffer.reset();
		}
		if (textureImage != nullptr) {
			textureImage.reset();
		}
	}

	void MeshObject::addVertexData(std::vector<Render::Vertex>& verticesInput, std::vector<uint32_t> indicesInput) 
	{
		vertices = verticesInput;
		indices = indicesInput;
		isEmpty = vertices.empty() || indices.empty();

		buffer->loadData(vertices, indices);
	}

	void MeshObject::Register(Renderer* renderer)
	{
		if (isEmptyMesh()) {
			Alert("Cannot register empty mesh buffer!", FATAL);
			return;
		}
		descriptorSet->addDescriptors({ uniform->getUUID(), textureImage->getUUID() });

		renderer->context().Load(buffer);
		renderer->context().Load(descriptorSet);
	}

	void MeshObject::Update(Renderer* renderer)
	{
		rotate(renderer->timer.getDeltaTimeSeconds() * 0.25 * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		uniform->setData(mvpBufferData);
	}

	void MeshObject::loadTextureFromFile(const std::string filePath)
	{
		textureImage->storeFilePath(filePath);
	}

	void MeshObject::loadMeshFromFile(const std::string filePath)
	{
		auto file = Request<FILETYPE>(FILE_Request, filePath, {Manager::FileFlags::READ | Manager::FileFlags::MODEL});

		if (file.wait() != Manager::State::YES) {
			Alert("Could not open mesh file.", CRITICAL);
			return;
		}

		auto meshFile = dynamic_cast<Manager::ModelFile*>(*file);

		std::unordered_map<Render::Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : meshFile->shapes) {
			for (const auto& index : shape.mesh.indices) {
				Render::Vertex vertex{};
				vertex.position = {
					meshFile->attrib.vertices[3 * index.vertex_index + 0],
					meshFile->attrib.vertices[3 * index.vertex_index + 1],
					meshFile->attrib.vertices[3 * index.vertex_index + 2]
				};
				vertex.normal = {
					meshFile->attrib.normals[3 * index.normal_index + 0],
					meshFile->attrib.normals[3 * index.normal_index + 1],
					meshFile->attrib.normals[3 * index.normal_index + 2]
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