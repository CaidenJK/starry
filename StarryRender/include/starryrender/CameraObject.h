#pragma once

#include "Asset.h"

namespace StarryRender 
{
	class CameraObject : public RenderAsset {
	public:
		CameraObject();
		~CameraObject();

		glm::mat4& getViewMatrix() { return localToGlobalSpace; }
		glm::mat4& getProjectionMatrix() { return projectionMatrix; }

		void setDimensions(const glm::vec2& dimensionsInput) { dimensions = dimensionsInput; }
		glm::vec2& getDimensions() { return dimensions; }

		const std::string getAssetName() override { return "CameraObject"; }
	private:
		// Inverse view matrix
		glm::mat4 localToGlobalSpace;
		glm::mat4 projectionMatrix;

		glm::vec2 dimensions;
	};
}