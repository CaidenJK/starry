#include "CameraObject.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Starry
{
	CameraObject::CameraObject()
	{
		localToGlobalSpace = glm::translate(localToGlobalSpace, glm::vec3(0.0, 0.0, -2.5));
		calculateProjectionMatrix();
	}

	CameraObject::~CameraObject()
	{
	}

	void CameraObject::calculateProjectionMatrix()
	{
		float aspectRatio = static_cast<float>(dimensions[0]) / static_cast<float>(dimensions[1]);
		projectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
		projectionMatrix[1][1] *= -1; // Invert Y for Vulkan
	}
}