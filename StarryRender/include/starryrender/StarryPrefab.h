#pragma once

#include "Asset.h"

namespace StarryRender {
	struct MVPMatrix {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class StarryPrefab : public RenderAsset {
	public:
		void Bind();

		const std::string getAssetName() override { return "Raw Starry Prefab"; }
	private:
		MVPMatrix mvpMatrix;
	};
}