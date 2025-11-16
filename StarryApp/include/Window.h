#pragma once
#include <GLFW/glfw3.h>

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_TITLE "Starry App"

namespace Starry {
	class Window {
	public:
		Window();
		~Window();
		
		void Init();
		void Close();
	private:
		int m_width;
		int m_height;
		const char* m_title;
		GLFWwindow* m_window; // correct type
	};
}