#pragma once

#include <StarryManager.h>
#include <StarryRender.h>

#include "CoreDLL.h"

namespace Starry
{
	class STARRY_API RenderWindow : public StarryAsset
	{
		public:
			RenderWindow(std::string name);
			~RenderWindow();

			void pollEvents() { window->pollEvents(); }
			bool shouldClose() { return window->shouldClose(); }

			std::shared_ptr<Window>& getWindow() { return window; }

			ASSET_NAME("Render Window");
		private:
			std::shared_ptr<Window> window = nullptr;
	};
}