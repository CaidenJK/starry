#include "Canvas.h"

#include "Device.h"

#include "Pipeline.h"
#include "SwapChain.h"
#include "Descriptor.h"

namespace StarryRender
{
    Canvas::Canvas()
    {

    }

    Canvas::~Canvas()
    {

    }

    void Canvas::constructImGuiInfo(Pipeline& pipeline, SwapChain& swapChain, Descriptor& descriptor)
    {
        (*device).fillImGuiInfo(&guiInfo);
        guiInfo.DescriptorPool = descriptor.getPool();
        guiInfo.MinImageCount = swapChain.getImageCount();
        guiInfo.ImageCount = swapChain.getImageCount();
        guiInfo.PipelineInfoMain.RenderPass = pipeline.getRenderPass();
    }

    void Canvas::init(size_t deviceUUID, CanvasConstructInfo info)
    {
        device = requestResource<Device>(deviceUUID, "self");
        window = requestResource<Window>(info.windowUUID, "self");

        auto pipeline = requestResource<Pipeline>(info.pipelineUUID, "self");
        auto swapChain = requestResource<SwapChain>(info.swapChainUUID, "self");
        auto descriptor = requestResource<Descriptor>(info.descriptorUUID, "self");

        if (device.wait() != ResourceState::YES ||
            window.wait() != ResourceState::YES ||
            pipeline.wait() != ResourceState::YES ||
            swapChain.wait() != ResourceState::YES ||
            descriptor.wait() != ResourceState::YES ) {
            Alert("Dependencies passed are not valid!", CRITICAL);
            return;
        }

        constructImGuiInfo(*pipeline, *swapChain, *descriptor);
        initImGui();
    }

    void Canvas::initImGui() 
    { 
        ImGui::CreateContext();
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= 
        ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_NavEnableGamepad |
        ImGuiConfigFlags_DockingEnable |
        ImGuiConfigFlags_ViewportsEnable;

        int width, height;
        (*window).getFramebufferSize(width, height);

        io.DisplaySize.x = (float)width;
        io.DisplaySize.y = (float)height;

        setImGuiStyle();

        ImGui_ImplGlfw_InitForVulkan((*window).getGLFWwindow(), true);
        ImGui_ImplVulkan_Init(&guiInfo);
    }

    void Canvas::setImGuiStyle()
    {
        ImGui::StyleColorsDark();

        ImGui::GetStyle().FontScaleMain = 1.0f;
    }

    void Canvas::destroy()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Canvas::PollEvents()
    {
        StartDraw();
        EndDraw();
    }

    void Canvas::StartDraw()
    {
        if (drawStage.load() != 0) return;
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame(); // Main thread
        ImGui::NewFrame();

        drawStage.store(1);
    }

    void Canvas::Record(VkCommandBuffer commandBuffer) // TODO: mutex for entire function
    {
        if (drawStage.load() != 1) return;
        Display(); // User API
        ImGui::Render();
        drawData = ImGui::GetDrawData(); // store
        if (drawData) ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer); // Can do this without updating
        drawStage.store(2);

        /*
            TODO: This guard allows some frames to be drawn without a gui. This appears as flickering.
            We should write intermediete frames to a image texture and draw that if Record is skipped.
        */
    }

    void Canvas::EndDraw()
    {
        if (drawStage.load() != 2) return;
        ImGui::EndFrame();
        ImGui::UpdatePlatformWindows(); // Main thread
        ImGui::RenderPlatformWindowsDefault();

        drawStage.store(0);
    }
}