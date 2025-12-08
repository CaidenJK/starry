#pragma once

#ifdef _WIN64

#include "starryrender/RenderContext.h"

using Window = StarryRender::Window;
using Vertex = StarryRender::Vertex;
using VertexBuffer = StarryRender::VertexBuffer;
using UniformBufferData = StarryRender::UniformBufferData;
using UniformBuffer = StarryRender::UniformBuffer;

#else

#error "Starry Render only supports Windows 64-bit systems."

#endif