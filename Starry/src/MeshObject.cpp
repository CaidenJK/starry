#include "MeshObject.h"

#include <glm/gtc/matrix_transform.hpp>

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; }

namespace Starry
{
	MeshObject::MeshObject(std::string nameInput) : name(nameInput) 
	{

	}

	MeshObject::~MeshObject() 
	{
		vertexBuffer.reset();
		imageBuffer.reset();
	}

	void MeshObject::addVertexData(std::vector<Vertex>& verticesInput, std::vector<uint32_t> indicesInput) 
	{
		vertices = verticesInput;
		indices = indicesInput;
		isEmpty = vertices.empty() || indices.empty();
	}

	void MeshObject::registerMeshBuffer(std::unique_ptr<RenderContext>& renderContext)
	{
		if (isEmptyMesh()) {
			registerAlert("Cannot register empty mesh buffer!", FATAL);
			return;
		}
		vertexBuffer = std::make_shared<VertexBuffer>();

		vertexBuffer->loadData(vertices, indices);
		renderContext->loadVertexBuffer(vertexBuffer);
	}

	void MeshObject::registerMeshBuffer(std::shared_ptr<RenderContext>& renderContext)
	{
		if (isEmptyMesh()) {
			registerAlert("Cannot register empty mesh buffer!", FATAL);
			return;
		}
		vertexBuffer = std::make_shared<VertexBuffer>();

		vertexBuffer->loadData(vertices, indices);
		renderContext->loadVertexBuffer(vertexBuffer);

		imageBuffer->loadFromFile(filePath);
		renderContext->loadTextureImage(imageBuffer);
	}

	void MeshObject::rotateMesh(float angleRadians, const glm::vec3& axis) {
		localToGlobalSpace = glm::rotate(localToGlobalSpace, angleRadians, axis);
	}

	void MeshObject::loadDiffuseTextureFromFile(const std::string filePath)
	{
		this->filePath = filePath;
		imageBuffer = std::make_shared<TextureImage>();
	}

	// Fix colors later

	void MeshObject::primitiveCube(MeshObject& obj, float size) {
		std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.5f}, CYAN_COLOR},
			{{-0.5f, 0.5f, 0.5f}, BLUE_COLOR},
			{{0.5f, 0.5f, 0.5f}, GREEN_COLOR},
			{{0.5f, -0.5f, 0.5f}, BLACK_COLOR},
			{{ -0.5f, -0.5f, -0.5f }, RED_COLOR},
			{{-0.5f, 0.5f, -0.5f}, YELLOW_COLOR},
			{{0.5f, 0.5f, -0.5f}, MAGENTA_COLOR},
			{{0.5f, -0.5f, -0.5f}, RED_COLOR}
		};
		const std::vector<uint32_t> indices = {
			0, 1, 2, 0, 2, 3,
			4, 5, 1, 4, 1, 0,
			7, 4, 0, 7, 0, 3,
			3, 2, 6, 3, 6, 7,
			5, 6, 2, 5, 2, 1,
			7, 6, 5, 7, 5, 4
		};

		for (auto& vertex : vertices) {
			vertex.position *= size;
		}

		obj.addVertexData(vertices, indices);
	}

	void MeshObject::primitiveQuad(MeshObject& obj, float width, float height) {
		std::vector<Vertex> vertices = {
			{{-0.5f * width, -0.5f * height, 0.0f}, CYAN_COLOR, {0, 1}},
			{{-0.5f * width, 0.5f * height, 0.0f}, BLUE_COLOR, { 0, 0}},
			{{0.5f * width, 0.5f * height, 0.0f}, GREEN_COLOR, {1, 0}},
			{{0.5f * width, -0.5f * height, 0.0f}, BLACK_COLOR, {1, 1}},
		};
		const std::vector<uint32_t> indices = {
			0, 1, 2, 0, 2, 3,
		};

		obj.addVertexData(vertices, indices);
	}

	void MeshObject::twoQuadTest(MeshObject& obj) {
		std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, CYAN_COLOR, {0, 1}},
			{{-0.5f, 0.5f, 0.0f}, BLUE_COLOR, { 0, 0}},
			{{0.5f, 0.5f, 0.0f}, GREEN_COLOR, {1, 0}},
			{{0.5f, -0.5f, 0.0f}, BLACK_COLOR, {1, 1}},

			{{-0.5f, -0.5f, -0.5f}, CYAN_COLOR, {0, 1}},
			{{-0.5f, 0.5f, -0.5f}, BLUE_COLOR, { 0, 0}},
			{{0.5f, 0.5f, -0.5f}, GREEN_COLOR, {1, 0}},
			{{0.5f, -0.5f, -0.5f}, BLACK_COLOR, {1, 1}},
		};

		const std::vector<uint32_t> indices = {
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7
		};

		obj.addVertexData(vertices, indices);
	}
}