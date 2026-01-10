#include "Interface.h"

namespace Editor
{
    Interface::Interface()
    {
    }

    Interface::~Interface()
    {
    }

    void Interface::loadTimer(size_t rendererUUID)
    {
        timer = requestResource<Timer>(rendererUUID, "timer");
        auto result = timer.wait();
    }

    void Interface::Display()
    {
        //ImGui::ShowDemoWindow();

        if (timer) {
            std::string message = std::string("Frame Metrics:\n") + 
                "FPS: " +
                std::to_string((*timer).getFPS()) +
                ", MS/frame: " +
                std::format("{:0<4.3}", (*timer).getDeltaTimeMilliSeconds());
            Overlay(message);
        }
        else {
            Overlay("N/A\n");
        }
    }

    void Interface::Overlay(std::string message)
    {
        // Center at Top Left
        int location = 0;
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; 
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        //
    
        ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_AlwaysAutoResize | 
            ImGuiWindowFlags_NoSavedSettings | 
            ImGuiWindowFlags_NoFocusOnAppearing | 
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove;

        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGui::Begin("Overlay", nullptr, window_flags);

        ImGui::Text("%s", message.c_str());
        ImGui::Separator();
        ImGui::End();
    }
}