#include "CameraObject.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StarryRender
{
	CameraObject::CameraObject()
	{
		localToGlobalSpace = glm::translate(localToGlobalSpace, glm::vec3(0.0, 0.0, -3.0));
		calculateProjectionMatrix();
	}

	CameraObject::~CameraObject()
	{
	}

	void CameraObject::calculateProjectionMatrix()
	{
		float aspectRatio = static_cast<float>(dimensions.width) / static_cast<float>(dimensions.height);
		projectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
		projectionMatrix[1][1] *= -1; // Invert Y for Vulkan
	}
}