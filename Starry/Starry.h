#pragma once 

#include "include/Scene.h"
#include "include/Renderer.h"
#include "include/SceneObject.h"
#include "include/MeshObject.h"
#include "include/CameraObject.h"

#include "include/Timer.h"
#include "include/Interface.h"
#include "include/Window.h"

#include "ManagedObject.h"

using Scene = Starry::Scene;
using Renderer = Starry::Renderer;
using SceneObject = Starry::SceneObject;
using MeshObject = Starry::MeshObject;
using CameraObject = Starry::CameraObject;
using Timer = Starry::Timer;

using UIElement = Starry::UIElement;
using RenderWindow = Starry::RenderWindow;

using ManagedObject = Starry::ManagedObject;

#define STARRY_INCLUDE