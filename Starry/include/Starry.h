#pragma once 

#include "starry/Scene.h"
#include "starry/Renderer.h"
#include "starry/SceneObject.h"
#include "starry/MeshObject.h"
#include "starry/CameraObject.h"

#include "starry/Timer.h"
#include "starry/Interface.h"

#include "starry/ManagedObject.h"

namespace Starry 
{
    using AssetManager = Manager::AssetManager;
    using ResourceAsk = Manager::ResourceAsk;

    template <typename T>
    using ResourceHandle = Manager::ResourceHandle<T>;
}

#define STARRY_INCLUDE