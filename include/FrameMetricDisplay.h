#pragma once

#include <Starry.h>

namespace Editor
{
    class FrameMetricDisplay : public UIElement
    {
        public:
            FrameMetricDisplay() {};
            ~FrameMetricDisplay() {};

            const std::string getAssetName() override { return "Frame Metric"; }

            void Init(size_t rendererUUID);
            void Draw() override;
        private:
            void Overlay(std::string message);

            ResourceHandle<Timer> timer;
    };
}