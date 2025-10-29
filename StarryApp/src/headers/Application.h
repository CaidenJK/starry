#pragma once
#include <iostream>
#include "Scene.h"
#include "Window.h"

namespace Starry {
	class Application {
		public:
			Application();
			~Application();


		private:
			//AssetManager m_assetManager; // ID Manager and Dependency Injector
			//FileHandeler m_fileHandeler;
			//ErrorHandeler m_errorHandeler;
			Window m_window;
			Scene m_currentScene;
			//Benchmarker
	};
}