#pragma once

#include <Starry.h>

namespace Editor
{
    class Interface : public Canvas
    {
        public:
            Interface();
            ~Interface();

            void loadTimer(size_t rendererUUID);

            const std::string getAssetName() override { return "Interface"; }
        private:
            void Display() override;

            void Overlay(std::string message);
            ResourceHandle<Timer> timer;
    };
}