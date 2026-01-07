#pragma once

#include <StarryManager.h>

#include "Window.h"
#include "RenderDevice.h"
#include "UniformBuffer.h"
#include "VertexBuffer.h"
#include "ImageBuffer.h"
#include "TextureImage.h"
#include "Canvas.h"

#define DEFAULT_SHADER_PATHS {}

namespace StarryRender
{
	/*
		The Starry Render Engine API class. Manages and interfaces with the Vulkan rendering context.
	*/

	namespace RenderConfigOptions {
		enum MSAAOptions {
			MSAA_DISABLED = VK_SAMPLE_COUNT_1_BIT,
			MSAA_2X = VK_SAMPLE_COUNT_2_BIT,
			MSAA_4X = VK_SAMPLE_COUNT_4_BIT,
			MSAA_8X = VK_SAMPLE_COUNT_8_BIT,
			MSAA_16X = VK_SAMPLE_COUNT_16_BIT,
			MSAA_32X = VK_SAMPLE_COUNT_32_BIT,
			MSAA_64X = VK_SAMPLE_COUNT_64_BIT
		};

		RenderConfig constructRenderConfig(std::string vertShader, std::string fragShader, MSAAOptions msaa, bool hasGUI);
	}

	class RenderContext : public StarryAsset
	{
	public:
		RenderContext() = default;
		~RenderContext();

		void Init(std::shared_ptr<Window>& window, RenderConfig config);

		void LoadBuffers();
		
		void Draw();

		void WaitIdle() { m_renderDevice->WaitIdle(); }
		
		void Destroy();

		std::array<int, 2> getExtent();
		
		void loadVertexBuffer(std::shared_ptr<VertexBuffer>& vertexBuffer);
		void loadVertexBuffer(std::shared_ptr<VertexBuffer>& vertexBuffer, size_t index);
		void clearVertexBuffers() { m_vertexBuffers.clear(); }

		void loadTextureImage(std::shared_ptr<TextureImage>& imageBuffer);

		void loadUniformBuffer(std::unique_ptr<UniformBuffer>& uniformBuffer);
		void updateUniformBuffer(UniformBufferData& buffer);

		void loadCanvas(std::shared_ptr<Canvas>& canvasRef) { canvas = canvasRef; };

		bool getErrorState() const { return AssetManager::get().lock()->isFatal(); }
		void dumpAlerts() const { AssetManager::get().lock()->isFatal(); }

		//void windowPollEvents() const { m_window->pollEvents(); }
		//bool windowShouldClose() const { return m_window->shouldClose(); }

		ASSET_NAME("RenderContext")
	private:
		const static int MAX_VERTEX_BUFFERS = 1;
		void initRenderDevice(std::shared_ptr<Window>& window);

		bool m_isInitialized = false;

		RenderConfig m_config;

		std::unique_ptr<RenderDevice> m_renderDevice = nullptr;

		std::shared_ptr<UniformBuffer> m_uniformBuffer = nullptr;
		std::shared_ptr<TextureImage> m_textureImage = nullptr;
		std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers = {};

		std::shared_ptr<Canvas> canvas = nullptr;
	};

	// call init
	// load resources
	// draw, finalize resources before first draw. If resources change, isinit = false

}