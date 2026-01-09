#pragma once

#include <StarryManager.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>

#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include "Window.h"

namespace StarryRender
{
    struct ImGUI_Config
    {
        std::weak_ptr<Window> window;
        ImGui_ImplVulkan_InitInfo initInfo;
    };

    class Canvas : public StarryAsset
    {
        public:
            Canvas();
            virtual ~Canvas();

            static bool isMouseControlledByImGUI();

            void registerCanvas(ImGUI_Config config); // Use asset manager

            void Record(VkCommandBuffer commandBuffer);

            void Destroy();

            virtual ASSET_NAME("Canvas");
        protected:
            virtual void Display() {};
        private:
            void initImGUI();
            void setImGUIStyle();

            ImGUI_Config config;

            ResourceHandle<VkDevice> device;
    };
}