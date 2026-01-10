#pragma once

#include <Starry.h>

namespace Editor
{
    class FrameMetricDisplay : public UIElement
    {
        public:
            FrameMetricDisplay() {};
            ~FrameMetricDisplay() {};

            void Init(size_t rendererUUID);
            void Draw() override;

            OBJECT_NAME("Frame Metric");
        private:
            void Overlay(std::string message);

            ResourceHandle<Timer> timer;
    };
}