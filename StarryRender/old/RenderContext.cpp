#include "RenderContext.h"

#define ERROR_CHECK if (getRenderErrorState()) { return; }
#define EXTERN_ERROR(x) if(x->getAlertSeverity() == StarryAsset::FATAL) { return; }

#define STARRY_RENDER_INITIALIZE_SUCCESS \
	"----------------------------------------\n" \
	"Starry Render initialized successfully!\n" \
	"----------------------------------------\n"

#define STARRY_RENDER_EXIT_SUCCESS \
	"----------------------------------------\n" \
	"Starry Render exited successfully!\n" \
	"----------------------------------------\n"

namespace StarryRender {

	namespace RenderConfigOptions {
		RenderConfig constructRenderConfig(std::string vertShader, std::string fragShader, MSAAOptions msaa, bool hasGUI)
		{
			return RenderConfig{ vertShader, fragShader, (VkSampleCountFlagBits)msaa, hasGUI };
		}
	}

	RenderContext::~RenderContext() 
	{
		Destroy();
		if (!getErrorState()) {
			Alert(STARRY_RENDER_EXIT_SUCCESS, BANNER);
		}
	}

	void RenderContext::Init(std::shared_ptr<Window>& window, RenderConfig config)
	{
		m_config = config;
		initRenderDevice(window);
	}

	void RenderContext::Draw() 
	{
		if (!m_isInitialized) {
			Alert("RenderContext not initialized before drawing!", FATAL);
			return;
		}
		m_renderDevice->Draw();
	}

	void RenderContext::Destroy()
	{
		for (auto& vb : m_vertexBuffers) {
			vb.reset();
		}

		m_uniformBuffer.reset();
		m_textureImage.reset();
		m_renderDevice.reset();
	}

	std::array<int, 2> RenderContext::getExtent()
	{
		if (m_renderDevice == nullptr) {
			Alert("Render device not initialized before getting extent!", CRITICAL);
			return { -1, -1 };
		}
		auto extent = requestResource<VkExtent2D>("Render Device", "Extent");
		if (m_renderDevice->getAlertSeverity() == StarryAsset::FATAL) {
			return { -1, -1 };
		}

		if (extent.wait() != ResourceState::YES) {
			Alert("Render Device is null or has errors when requesting an extent.", CRITICAL);
			return { -1, -1 };
		}
		return { static_cast<int>((*extent).width), static_cast<int>((*extent).height) };
	}

	void RenderContext::loadVertexBuffer(std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		if (m_vertexBuffers.size() >= MAX_VERTEX_BUFFERS) {
			Alert("Maximum number of vertex buffers reached! Can't load more.", CRITICAL);
			return;
		}

		m_vertexBuffers.push_back(vertexBuffer);

		if (m_vertexBuffers.back()->getNumVertices() == 0 || m_vertexBuffers.back()->getNumIndices() == 0) {
			Alert("No vertex or index data to attatch to mesh object!", CRITICAL);
			return;
		}
		if (m_renderDevice == nullptr) {
			Alert("Render Device is NULL, cannot load buffer.", FATAL);
			return;
		}
	}

	void RenderContext::loadVertexBuffer(std::shared_ptr<VertexBuffer>& vertexBuffer, size_t index)
	{
		if (index >= m_vertexBuffers.size()) {
			Alert("Index out of bounds when loading vertex buffer!", CRITICAL);
			return;
		}

		m_vertexBuffers[index] = vertexBuffer;

		if (m_renderDevice == nullptr) {
			Alert("Render Device is NULL, cannot load buffer.", FATAL);
			return;
		}
	}

	void RenderContext::loadTextureImage(std::shared_ptr<TextureImage>& imageBuffer)
	{
		m_textureImage = imageBuffer;
		if (m_renderDevice == nullptr) {
			Alert("Render Device is NULL, cannot load buffer.", FATAL);
			return;
		}
	}

	void RenderContext::loadUniformBuffer(std::unique_ptr<UniformBuffer>& uniformBuffer) 
	{
		m_uniformBuffer = std::move(uniformBuffer);
	}

	void RenderContext::updateUniformBuffer(UniformBufferData& buffer) 
	{
		m_uniformBuffer->setBuffer(buffer);
	}

	void RenderContext::initRenderDevice(std::shared_ptr<Window>& window) 
	{
		if (window == nullptr) {
			Alert("Window not set before creating device!", FATAL);
			return;
		}
		
		m_renderDevice = std::make_unique<RenderDevice>(window, m_config); EXTERN_ERROR(m_renderDevice);

		if (m_config.vertexShaderPath.empty() || m_config.fragmentShaderPath.empty()) {
			Alert("Shader paths not set before creating device!", FATAL);
			return;
		}
		
		m_renderDevice->LoadShader(m_config.vertexShaderPath, m_config.fragmentShaderPath); EXTERN_ERROR(m_renderDevice);
		m_renderDevice->InitDraw(); EXTERN_ERROR(m_renderDevice);

		if (!getErrorState()) {
			m_isInitialized = true;
			Alert(STARRY_RENDER_INITIALIZE_SUCCESS, BANNER);
		}
	}

	void RenderContext::LoadBuffers()
	{
		if (m_renderDevice == nullptr) {
			Alert("LoadBuffers was called before Init.", CRITICAL);
		}
		if (m_uniformBuffer == nullptr) {
			m_uniformBuffer = std::make_shared<UniformBuffer>();
		}
		m_renderDevice->loadUniformBuffer(m_uniformBuffer);
		m_renderDevice->loadImageBuffer(m_textureImage);
		m_renderDevice->setDescriptors();

		if (canvas) m_renderDevice->loadCanvas(canvas);

		for (auto buffer : m_vertexBuffers) {
			m_renderDevice->loadVertexBuffer(buffer);
		}
	}
}