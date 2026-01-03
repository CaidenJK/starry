#include "SceneObject.h"

namespace Starry
{
	void SceneObject::rotate(float angleRadians, const glm::vec3& axis)
	{
		mvpBufferData.model = glm::rotate(mvpBufferData.model, angleRadians, axis);
	}

	void SceneObject::translate(const glm::vec3& translation)
	{
		mvpBufferData.model = glm::translate(mvpBufferData.model, translation);
	}

	void SceneObject::scale(const glm::vec3& scaleFactors)
	{
		mvpBufferData.model = glm::scale(mvpBufferData.model, scaleFactors);
	}
}