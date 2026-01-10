#pragma once

#include <StarryManager.h>
#include <StarryRender.h>

#include "Timer.h"

namespace Starry
{
    class UIElement : public Manager::StarryAsset
    {
        public:
            UIElement() = default;
            ~UIElement() = default;

            virtual void Draw() = 0;

            virtual ASSET_NAME("UI Element");
    };

    class Interface : public Render::Canvas
    {
        public:
            Interface();
            ~Interface();

            void loadElement(std::shared_ptr<UIElement>& element, int drawOrder);

            const std::string getAssetName() override { return "Interface"; }
        private:
            virtual void Display() override;

            std::map<int, std::shared_ptr<UIElement>> elements;
            Manager::ResourceHandle<Timer> timer;
    };
}