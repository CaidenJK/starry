#include "MeshObject.h"

#include <glm/gtc/matrix_transform.hpp>

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; }

namespace StarryRender 
{
	MeshObject::MeshObject(std::string nameInput) : name(nameInput) 
	{

	}

	MeshObject::~MeshObject() 
	{
		vertexBuffer.reset();
	}

	void MeshObject::addVertexData(std::vector<Vertex>& verticesInput, std::vector<uint32_t> indicesInput) 
	{
		vertices = verticesInput;
		indices = indicesInput;
		isEmpty = vertices.empty() || indices.empty();
	}

	void MeshObject::attatchBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice) 
	{
		if (vertices.empty() || indices.empty()) {
			registerAlert("No vertex or index data to attatch to mesh object!", CRITICAL);
			return;
		}
		if (vertexBuffer != VK_NULL_HANDLE) {
			registerAlert("AttatchBuffer called more than once! All calls other than the first are skipped.", WARNING);
			return;
		}
		vertexBuffer = std::make_shared<VertexBuffer>(device); EXTERN_ERROR(vertexBuffer);
		vertexBuffer->loadData(physicalDevice, vertices, indices);
	}

	void MeshObject::rotateMesh(float angleRadians, const glm::vec3& axis) {
		localToGlobalSpace = glm::rotate(localToGlobalSpace, angleRadians, axis);
	}

	// Fix colors later

	MeshObject MeshObject::primitiveCube(float size) {
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

		MeshObject meshObject("Cube Primitive");
		meshObject.addVertexData(vertices, indices);
		if (meshObject.getAlertSeverity() == FATAL) { return {}; }
		return meshObject;
	}

	MeshObject MeshObject::primitiveQuad(float width, float height) {
		std::vector<Vertex> vertices = {
			{{-0.5f * width, -0.5f * height, 0.0f}, CYAN_COLOR},
			{{-0.5f * width, 0.5f * height, 0.0f}, BLUE_COLOR},
			{{0.5f * width, 0.5f * height, 0.0f}, GREEN_COLOR},
			{{0.5f * width, -0.5f * height, 0.0f}, BLACK_COLOR},
		};
		const std::vector<uint32_t> indices = {
			0, 1, 2, 0, 2, 3,
		};

		MeshObject meshObject("Quad Primitive");
		meshObject.addVertexData(vertices, indices);
		if (meshObject.getAlertSeverity() == FATAL) { return {}; }
		return meshObject;
	}
}