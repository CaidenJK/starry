#pragma once

#include <StarryRender.h>
#include <StarryAsset.h>

#include <array>

namespace Starry
{
	class CameraObject : public StarryAsset {
	public:
		CameraObject();
		~CameraObject();

		glm::mat4& getViewMatrix() { return localToGlobalSpace; }
		glm::mat4& getProjectionMatrix() { return projectionMatrix; }

		void setClippingPlanes(float nearInput, float farInput) { nearPlane = nearInput; farPlane = farInput; calculateProjectionMatrix(); }
		void setFOV(float fovInput) { FOV = fovInput; calculateProjectionMatrix(); }
		void setExtent(const std::array<int, 2>& dimensionsInput) { dimensions = dimensionsInput; calculateProjectionMatrix(); }

		const std::string getAssetName() override { return "CameraObject"; }
	private:
		void calculateProjectionMatrix();

		// Inverse view matrix
		glm::mat4 localToGlobalSpace = 1.0f;
		glm::mat4 projectionMatrix;

		float nearPlane = 0.1f;
		float farPlane = 100.0f;

		float FOV = 45.0f;

		std::array<int, 2> dimensions = { 800, 600 };
	};
}