#include "Canvas.h"

namespace StarryRender
{
    Canvas::Canvas()
    {

    }

    Canvas::~Canvas()
    {

    }

    void Canvas::registerCanvas(ImGUI_Config config)
    {  
        device = requestResource<VkDevice>("Render Device", "VkDevice");

        this->config = config;
        initImGUI();
    }

    void Canvas::initImGUI() 
    { 
        ImGui::CreateContext();
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= 
            ImGuiConfigFlags_NavEnableKeyboard |
            ImGuiConfigFlags_NavEnableGamepad |
            ImGuiConfigFlags_ViewportsEnable;


        if (auto wndw = config.window.lock()) {
            int width, height;
            wndw->getFramebufferSize(width, height);

            io.DisplaySize.x = (float)width;
            io.DisplaySize.y = (float)height;

            setImGUIStyle();

            ImGui_ImplGlfw_InitForVulkan(wndw->getGLFWwindow(), true);
        }
        else {
            registerAlert("Canvas was passed an invalid window!", FATAL);
            return;
        }
        ImGui_ImplVulkan_Init(&config.initInfo);
    }

    void Canvas::setImGUIStyle()
    {
        ImGui::StyleColorsDark();

        ImGui::GetStyle().FontScaleMain = 1.0f;
    }

    void Canvas::Destroy()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Canvas::Record(VkCommandBuffer commandBuffer)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        Display(); // User API

        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
    }
}