#pragma once

#include <StarryRender.h>
#include <StarryManager.h>

#include "Renderer.h"

namespace Starry
{
	class SceneObject : public Manager::StarryAsset
	{
		public:
			enum Type
			{
				MESH,
				CAMERA,
				NONE
			};
			
			SceneObject(Type type, std::string name) : type(type), name(name) {}
			~SceneObject() = default;

			virtual void Init() {}
			virtual void Register(Renderer* renderer) {}
			virtual void Update(Renderer* renderer) {}
			virtual void Destroy() {}

			std::string& getName() { return name; }

			void rotate(float angleRadians, const glm::vec3& axis);
			void translate(const glm::vec3& translation);
			void scale(const glm::vec3& scaleFactors);

			void setProjection(const glm::mat4& proj);
			void setView(const glm::mat4& view);

			Render::UniformData& getBufferData() { return mvpBufferData; }

			Type getType() {return type;}

			virtual ASSET_NAME(std::string("Scene object: ") + const_cast<std::string&>(name))
		protected:
			std::string name;

			Render::UniformData mvpBufferData = { 1.0f, 1.0f, 1.0f };
		private:
			Type type;
	};
}