#pragma once

#include <StarryManager.h>
#include <StarryRender.h>

#include "Timer.h"

namespace Starry
{
    class STARRY_API Interface : public Canvas
    {
        public:
            Interface();
            ~Interface();

            void loadTimer(size_t rendererUUID);

            const std::string getAssetName() override { return "Interface"; }
        private:
            virtual void Display() override;

            void Overlay(std::string message);
            ResourceHandle<Timer> timer;
    };
}