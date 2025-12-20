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
		m_renderDevice.reset();
	}

	std::array<int, 2> RenderContext::getExtent()
	{
		if (m_renderDevice == nullptr) {
			registerAlert("Render device not initialized before getting extent!", CRITICAL);
			return { -1, -1 };
		}
		auto extent = m_renderDevice->getExtent();
		if (m_renderDevice->getAlertSeverity() == StarryAsset::FATAL) {
			return { -1, -1 };
		}
		return { static_cast<int>(extent.width), static_cast<int>(extent.height) };
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

		if (m_vertexBuffers.back()->getNumVerticies() == 0 || m_vertexBuffers.back()->getNumIndicies() == 0) {
			registerAlert("No vertex or index data to attatch to mesh object!", CRITICAL);
			return;
		}
		m_renderDevice->LoadVertexBuffer(m_vertexBuffers.back()); EXTERN_ERROR(m_renderDevice); EXTERN_ERROR(vertexBuffer);
	}

	void RenderContext::loadVertexBuffer(std::shared_ptr<VertexBuffer>& vertexBuffer, size_t index)
	{
		if (index >= m_vertexBuffers.size()) {
			registerAlert("Index out of bounds when loading vertex buffer!", CRITICAL);
			return;
		}

		m_vertexBuffers[index] = vertexBuffer;
		m_renderDevice->LoadVertexBuffer(m_vertexBuffers[index]); EXTERN_ERROR(m_renderDevice); EXTERN_ERROR(vertexBuffer);
	}

	void RenderContext::loadUniformBuffer(std::unique_ptr<UniformBuffer>& uniformBuffer) 
	{
		m_uniformBuffer = std::move(uniformBuffer);
	}

	void RenderContext::initInternalWindow() 
	{
		m_window = std::make_shared<Window>("Starry Dev"); ERROR_CHECK;
	}

	std::shared_ptr<UniformBuffer> RenderContext::createUniformBuffer() 
	{
		if (m_renderDevice == nullptr) {
			registerAlert("Render device not initialized before creating uniform buffer!", CRITICAL);
			return nullptr;
		}
		auto uniformBuffer = std::make_shared<UniformBuffer>(m_renderDevice->getDevice());
		if (uniformBuffer->getAlertSeverity() == StarryAsset::FATAL) {
			return nullptr;
		}
		return uniformBuffer;
	}

	void RenderContext::updateUniformBuffer(UniformBufferData& buffer) 
	{
		m_uniformBuffer->setBuffer(buffer);
	}

	std::shared_ptr<VertexBuffer> RenderContext::createVertexBuffer() 
	{
		if (m_renderDevice == nullptr) {
			registerAlert("Render device not initialized before creating vertex buffer!", CRITICAL);
			return nullptr;
		}
		auto vertexBuffer = std::make_shared<VertexBuffer>(m_renderDevice->getDevice());
		if (vertexBuffer->getAlertSeverity() == StarryAsset::FATAL) {
			return nullptr;
		}
		return vertexBuffer;
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
		if (m_uniformBuffer == nullptr) {
			m_uniformBuffer = createUniformBuffer();
		}
		m_renderDevice->loadUniformBuffer(m_uniformBuffer);
		
		m_renderDevice->LoadShader(m_shaderPaths[0], m_shaderPaths[1]); EXTERN_ERROR(m_renderDevice);
		m_renderDevice->InitDraw(); EXTERN_ERROR(m_renderDevice);

		if (!getRenderErrorState()) {
			m_isInitialized = true;
			registerAlert(STARRY_RENDER_INITIALIZE_SUCCESS, BANNER);
		}
	}
}