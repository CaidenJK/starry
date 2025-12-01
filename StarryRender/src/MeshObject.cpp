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
}