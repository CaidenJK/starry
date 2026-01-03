#include "CameraObject.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Starry
{
	CameraObject::CameraObject(std::string name) : SceneObject(std::string("Camera, ") + name)
	{
	}

	CameraObject::~CameraObject()
	{
	}

	void CameraObject::Init() 
	{
		mvpBufferData.model = glm::translate(mvpBufferData.model, glm::vec3(0.0, 0.0, -3.0));
		mvpBufferData.view = mvpBufferData.model;
		calculateProjectionMatrix();
	}

	void CameraObject::Register(std::shared_ptr<RenderContext>& renderContext)
	{

	}

	void CameraObject::Update(std::shared_ptr<RenderContext>& renderContext) 
	{
		setExtent(renderContext->getExtent());
		//calculateProjectionMatrix();
	}

	void CameraObject::calculateProjectionMatrix()
	{
		float aspectRatio = static_cast<float>(dimensions[0]) / static_cast<float>(dimensions[1]);
		mvpBufferData.proj = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
		mvpBufferData.proj[1][1] *= -1; // Invert Y for Vulkan
	}
}