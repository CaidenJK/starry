#include "MeshObject.h"

#define EXTERN_ERROR(x) if(x->getAlertSeverity() == FATAL) { return; }

namespace StarryRender {
	MeshObject::MeshObject(std::string nameInput) : name(nameInput) {

	}

	MeshObject::~MeshObject() {
		vertexBuffer.reset();
	}

	void MeshObject::addVertexData(std::vector<Vertex>& verticiesInput, std::vector<uint32_t> indiciesInput) {
		verticies = verticiesInput;
		indicies = indiciesInput;
	}

	void MeshObject::attatchBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice) {
		if (verticies.empty() || indicies.empty()) {
			registerAlert("No vertex or index data to attatch to mesh object!", CRITICAL);
			return;
		}
		if (vertexBuffer != VK_NULL_HANDLE) {
			registerAlert("AttatchBuffer called more than once! All calls other than the first are skipped.", WARNING);
			return;
		}
		vertexBuffer = std::make_shared<VertexBuffer>(device); EXTERN_ERROR(vertexBuffer);
		vertexBuffer->loadData(physicalDevice, verticies, indicies);
	}
}