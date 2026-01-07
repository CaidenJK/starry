#include "Interface.h"

namespace Editor
{
    Interface::Interface() {}
    Interface::~Interface() {}

    void Interface::Display()
    {
        ImGui::ShowDemoWindow();
    }
}