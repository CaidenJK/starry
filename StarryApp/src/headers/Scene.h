#pragma once
#include "Renderer.h"

namespace Starry {
	class Scene {
	public:
		Scene();
		~Scene();

		void addRenderer(const Renderer& renderer);
	private:
		Renderer* m_renderer;
		int64_t m_ID;
		//Objects m_objects;
	};
}