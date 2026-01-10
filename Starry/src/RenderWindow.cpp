#include "RenderWindow.h"

namespace Starry
{
	RenderWindow::RenderWindow(std::string name)
	{
		window = std::make_shared<Window>(name.c_str());
	}

	RenderWindow::~RenderWindow() {}
}