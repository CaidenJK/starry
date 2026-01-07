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
        protected:
            void Display() override;
        private:

    };
}
