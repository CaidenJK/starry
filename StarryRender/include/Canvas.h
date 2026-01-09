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
    struct CanvasConstructInfo
    {
        size_t windowUUID;
    };

    class Device;

    class Canvas : public StarryAsset
    {
        public:
            Canvas();
            virtual ~Canvas();

            void init(size_t deviceUUID, CanvasConstructInfo info);
            void destroy();

            static bool isMouseControlledByImGUI();

            void Record(VkCommandBuffer commandBuffer);

            virtual ASSET_NAME("Canvas");
        protected:
            virtual void Display() {};
        private:
            void initImGUI();
            void setImGUIStyle();

            //ImGUI_Config config;

            ResourceHandle<Device> device;
            ResourceHandle<Window> window;
    };
}