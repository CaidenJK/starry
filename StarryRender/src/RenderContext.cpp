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

	RenderContext::~RenderContext() 
	{
		Destroy();
		if (!getRenderErrorState()) {
			registerAlert(STARRY_RENDER_EXIT_SUCCESS, BANNER);
		}
	}

	void RenderContext::Init()
	{
		initInternalWindow();
		initRenderDevice();
	}

	void RenderContext::Init(std::shared_ptr<Window>& window)
	{
		m_window = window;
		initRenderDevice();
	}

	void RenderContext::Draw() 
	{
		if (!m_isInitialized) {
			initRenderDevice();
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
			registerAlert("Render device not initialized before getting extent!", CRITICAL);
			return { -1, -1 };
		}
		auto extent = requestResource<VkExtent2D>("Render Device", "Extent");
		if (m_renderDevice->getAlertSeverity() == StarryAsset::FATAL) {
			return { -1, -1 };
		}

		if (extent.wait() != ResourceState::YES) {
			registerAlert("Render Device is null or has errors when requesting an extent.", CRITICAL);
			return { -1, -1 };
		}
		return { static_cast<int>((*extent).width), static_cast<int>((*extent).height) };
	}

	void RenderContext::loadShaders(const std::string& vertShaderPath, const std::string& fragShaderPath) 
	{
		m_shaderPaths[0] = vertShaderPath;
		m_shaderPaths[1] = fragShaderPath;
	}

	void RenderContext::loadShaders(std::array<std::string, 2>& shaders) 
	{
		m_shaderPaths = shaders;
	}

	void RenderContext::loadVertexBuffer(std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		if (m_vertexBuffers.size() >= MAX_VERTEX_BUFFERS) {
			registerAlert("Maximum number of vertex buffers reached! Can't load more.", CRITICAL);
			return;
		}

		m_vertexBuffers.push_back(vertexBuffer);

		if (m_vertexBuffers.back()->getNumVertices() == 0 || m_vertexBuffers.back()->getNumIndices() == 0) {
			registerAlert("No vertex or index data to attatch to mesh object!", CRITICAL);
			return;
		}
		if (m_renderDevice == nullptr) {
			registerAlert("Render Device is NULL, cannot load buffer.", FATAL);
			return;
		}
	}

	void RenderContext::loadVertexBuffer(std::shared_ptr<VertexBuffer>& vertexBuffer, size_t index)
	{
		if (index >= m_vertexBuffers.size()) {
			registerAlert("Index out of bounds when loading vertex buffer!", CRITICAL);
			return;
		}

		m_vertexBuffers[index] = vertexBuffer;

		if (m_renderDevice == nullptr) {
			registerAlert("Render Device is NULL, cannot load buffer.", FATAL);
			return;
		}
	}

	void RenderContext::loadTextureImage(std::shared_ptr<TextureImage>& imageBuffer)
	{
		m_textureImage = imageBuffer;
		if (m_renderDevice == nullptr) {
			registerAlert("Render Device is NULL, cannot load buffer.", FATAL);
			return;
		}
	}

	void RenderContext::loadUniformBuffer(std::unique_ptr<UniformBuffer>& uniformBuffer) 
	{
		m_uniformBuffer = std::move(uniformBuffer);
	}

	void RenderContext::initInternalWindow() 
	{
		m_window = std::make_shared<Window>("Starry Dev"); ERROR_CHECK;
	}

	void RenderContext::updateUniformBuffer(UniformBufferData& buffer) 
	{
		m_uniformBuffer->setBuffer(buffer);
	}

	void RenderContext::initRenderDevice() 
	{
		if (m_window == nullptr) {
			registerAlert("Window not set before creating device!", FATAL);
			return;
		}
		
		m_renderDevice = std::make_unique<RenderDevice>(m_window); EXTERN_ERROR(m_renderDevice);

		if (m_shaderPaths[0].empty() || m_shaderPaths[1].empty()) {
			registerAlert("Shader paths not set before creating device!", FATAL);
			return;
		}
		
		m_renderDevice->LoadShader(m_shaderPaths[0], m_shaderPaths[1]); EXTERN_ERROR(m_renderDevice);
		m_renderDevice->InitDraw(); EXTERN_ERROR(m_renderDevice);

		if (!getRenderErrorState()) {
			m_isInitialized = true;
			registerAlert(STARRY_RENDER_INITIALIZE_SUCCESS, BANNER);
		}
	}

	void RenderContext::LoadBuffers()
	{
		if (m_renderDevice == nullptr) {
			registerAlert("LoadBuffers was called before Init.", CRITICAL);
		}
		if (m_uniformBuffer == nullptr) {
			m_uniformBuffer = std::make_shared<UniformBuffer>();
		}
		m_renderDevice->loadUniformBuffer(m_uniformBuffer);
		m_renderDevice->loadImageBuffer(m_textureImage);
		m_renderDevice->setDescriptors();

		for (auto buffer : m_vertexBuffers) {
			m_renderDevice->loadVertexBuffer(buffer);
		}
	}
}