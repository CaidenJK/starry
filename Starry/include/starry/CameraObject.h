#pragma once

#include "SceneObject.h"

#include <array>

namespace Starry
{
	class CameraObject : public SceneObject {
		public:
			CameraObject(std::string name = "Default");
			~CameraObject();

			void Init() override;
			void Register(Renderer* renderer) override;
			void Update(Renderer* renderer) override;

			void setClippingPlanes(float nearInput, float farInput) { nearPlane = nearInput; farPlane = farInput; calculateProjectionMatrix(); }
			void setFOV(float fovInput) { FOV = fovInput; calculateProjectionMatrix(); }
			void setExtent(std::array<unsigned int, 2> dimensionsInput) { dimensions = dimensionsInput; calculateProjectionMatrix(); }

		private:
			void calculateProjectionMatrix();

			float nearPlane = 0.1f;
			float farPlane = 100.0f;

			float FOV = 45.0f;

			std::array<unsigned int, 2> dimensions = { 800, 600 };
	};
}