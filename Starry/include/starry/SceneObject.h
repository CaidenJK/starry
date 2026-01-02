#pragma once

#include <StarryRender.h>
#include <StarryManager.h>

namespace Starry
{
	class SceneObject : public StarryAsset
	{
		public:
			SceneObject(std::string name) : name(name) {}
			~SceneObject() = default;

			virtual void Init() {}
			virtual void Register(std::shared_ptr<RenderContext>& renderContext) {}
			virtual void Update(std::shared_ptr<RenderContext>& renderContext) {}
			virtual void Destroy() {}

			std::string& getName() { return name; }

			void rotate(float angleRadians, const glm::vec3& axis);
			void translate(const glm::vec3& translation);
			void scale(const glm::vec3& scaleFactors);

			UniformBufferData& getBufferData() { return mvpBufferData; }

			virtual ASSET_NAME(std::string("Scene object: ") + const_cast<std::string&>(name))
		protected:
			std::string name;

			UniformBufferData mvpBufferData = { 1.0f, 1.0f, 1.0f };
	};
}