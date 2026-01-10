#pragma once

#include <Starry.h>

namespace Editor
{
    class Interface : public Canvas
    {
        public:
            Interface();
            ~Interface();

            const std::string getAssetName() override { return "Interface"; }
        private:
            void Display() override;
    };
}