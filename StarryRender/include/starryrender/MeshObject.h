#pragma once

#include <string>

#include "Asset.h"
#include "VertexBuffer.h"

#define DEFAULT_NAME_MESH "MeshObject"

namespace StarryRender {
	class MeshObject : public RenderAsset {
	public:
		MeshObject(std::string nameInput = DEFAULT_NAME_MESH);
		~MeshObject();

		void addVertexData(std::vector<Vertex>& verticiesInput, std::vector<uint32_t> indiciesInput);
		void attatchBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice);

		glm::mat4& getModelMatrix() { return localToGlobalSpace; }

		std::shared_ptr<VertexBuffer>& getRawVertexBuffer() { return vertexBuffer; }
		const std::string getAssetName() override { return const_cast<std::string&>(name); }
	private:
		std::string name;

		glm::mat4 localToGlobalSpace;

		std::vector<Vertex> verticies;
		std::vector<uint32_t> indicies;
		std::shared_ptr<VertexBuffer> vertexBuffer;
	};
}