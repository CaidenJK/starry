#pragma once

#include <StarryManager.h>

#define OBJECT_NAME(x) \
    const std::string getAssetName() override { return x; }

namespace Starry
{
    class ManagedObject : public Manager::StarryAsset
    {
        public:
            ManagedObject() = default;
            ~ManagedObject() = default;

            virtual const std::string getAssetName() override = 0;
    };
}