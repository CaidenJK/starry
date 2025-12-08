#pragma once

#include "Window.h"
#include "RenderDevice.h"
#include "UniformBuffer.h"
#include "VertexBuffer.h"
#include "Asset.h"

#define DEFAULT_SHADER_PATHS {}

namespace Starry
{
	/*
		The Starry Render Engine API class. Manages and interfaces with the Vulkan rendering context.
	*/
	class RenderContext : public StarryRender::RenderAsset
	{
	public:
		RenderContext() = default;
		~RenderContext();

		void Init();
		void Init(std::shared_ptr<StarryRender::Window>& window);
		
		void Draw();

		void WaitIdle() { m_renderDevice->WaitIdle(); }
		
		void Destroy();

		std::array<int, 2> getExtent();

		void loadShaders(const std::string& vertShaderPath, const std::string& fragShaderPath);
		void loadShaders(std::array<std::string, 2>& shaders);
		
		std::shared_ptr<StarryRender::VertexBuffer> createVertexBuffer();
		void loadVertexBuffer(std::shared_ptr<StarryRender::VertexBuffer>& vertexBuffer);
		void loadVertexBuffer(std::shared_ptr<StarryRender::VertexBuffer>& vertexBuffer, size_t index);
		void clearVertexBuffers() { m_vertexBuffers.clear(); }
		
		std::shared_ptr<StarryRender::UniformBuffer> createUniformBuffer();
		void loadUniformBuffer(std::unique_ptr<StarryRender::UniformBuffer>& uniformBuffer);
		void updateUniformBuffer(StarryRender::UniformBufferData& buffer);

		bool getRenderErrorState() const { return StarryRender::ErrorHandler::get().lock()->isFatal(); }
		void dumpAlerts() const { StarryRender::ErrorHandler::get().lock()->isFatal(); }

		void windowPollEvents() const { m_window->pollEvents(); }
		bool windowShouldClose() const { return m_window->shouldClose(); }

		const std::string getAssetName() override { return "RenderContext"; }
	private:
		const static int MAX_VERTEX_BUFFERS = 1;
		void initInternalWindow();
		void initRenderDevice();

		bool m_isInitialized = false;

		std::unique_ptr<StarryRender::RenderDevice> m_renderDevice = nullptr;
		std::shared_ptr<StarryRender::Window> m_window = nullptr;

		std::shared_ptr<StarryRender::UniformBuffer> m_uniformBuffer = nullptr;

		std::vector<std::shared_ptr<StarryRender::VertexBuffer>> m_vertexBuffers = {};

		std::array<std::string, 2> m_shaderPaths = DEFAULT_SHADER_PATHS;
	};

	// call init
	// load resources
	// draw, finalize resources before first draw. If resources change, isinit = false

}