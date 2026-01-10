#pragma once

#include <Starry.h>

#include "FrameMetricDisplay.h"

#include <memory>
 
namespace Editor
{
    struct ApplicationConfig {
		bool enableFileLogging = false;
		bool managerExitRights = true;
		const std::string packageName = "starry-editor";
    };

    class Application : public ManagedObject {
    public:
        Application() {};
        ~Application() {};

		Application operator=(const Application&) = delete;
		Application(const Application&) = delete;

        void run();

        bool hasFatalError();

		static void setConfig(const ApplicationConfig& config);

		OBJECT_NAME("Application")

    private:
        void init();
        void mainLoop();
        void cleanup();

		std::shared_ptr<Renderer> m_renderer = nullptr;
		std::shared_ptr<RenderWindow> m_window = nullptr;
        std::shared_ptr<Scene> m_scene = nullptr;

        std::shared_ptr<FrameMetricDisplay> m_metricDisplay = nullptr;
    };
}