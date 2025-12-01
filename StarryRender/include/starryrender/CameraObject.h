#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Asset.h"

namespace StarryRender 
{
	class CameraObject : public RenderAsset {
	public:
		CameraObject();
		~CameraObject();

		glm::mat4& getViewMatrix() { return localToGlobalSpace; }
		glm::mat4& getProjectionMatrix() { return projectionMatrix; }

		void setClippingPlanes(float nearInput, float farInput) { nearPlane = nearInput; farPlane = farInput; calculateProjectionMatrix(); }
		void setFOV(float fovInput) { FOV = fovInput; calculateProjectionMatrix(); }
		void setExtent(const VkExtent2D& dimensionsInput) { dimensions = dimensionsInput; calculateProjectionMatrix(); }

		const std::string getAssetName() override { return "CameraObject"; }
	private:
		void calculateProjectionMatrix();

		// Inverse view matrix
		glm::mat4 localToGlobalSpace = 1.0f;
		glm::mat4 projectionMatrix;

		float nearPlane = 0.1f;
		float farPlane = 100.0f;

		float FOV = 45.0f;

		VkExtent2D dimensions = { 800, 600 };
	};
}