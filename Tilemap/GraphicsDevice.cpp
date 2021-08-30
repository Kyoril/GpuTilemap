
#include "GraphicsDevice.h"
#include "GraphicsContext.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "RenderWindow.h"
#include "ConstantBuffer.h"
#include "ComUtils.h"

GraphicsDevice::GraphicsDevice()
{
	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_12_0
	};

	ComPtr<ID3D11DeviceContext> immediateContext;
	HRESULT hr = ::D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		flags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &m_device, nullptr, &immediateContext);
	ThrowIfFail(hr);

	m_immediateContext = std::make_unique<GraphicsContext>(*m_device.Get(), immediateContext);
}

RenderWindow* GraphicsDevice::CreateRenderWindow(const TCHAR name[], uint16_t width, uint16_t height)
{
	auto window = std::make_unique<RenderWindow>(name, width, height, *m_device.Get());
	m_renderWindows.emplace_back(std::move(window));

	return m_renderWindows.back().get();
}

VertexShader* GraphicsDevice::CreateVertexShader(const void* data, const uint32_t dataSize)
{
	auto shader = std::make_unique<VertexShader>(*m_device.Get(), data, dataSize);
	m_vertexShader.emplace_back(std::move(shader));

	return m_vertexShader.back().get();
}

PixelShader* GraphicsDevice::CreatePixelShader(const void* data, const uint32_t dataSize)
{
	auto shader = std::make_unique<PixelShader>(*m_device.Get(), data, dataSize);
	m_pixelShader.emplace_back(std::move(shader));

	return m_pixelShader.back().get();
}

ConstantBuffer* GraphicsDevice::CreateConstantBuffer(uint32_t size, const void* initialData)
{
	auto buffer = std::make_unique<ConstantBuffer>(*m_device.Get(), size, initialData);
	m_constantBuffers.emplace_back(std::move(buffer));

	return m_constantBuffers.back().get();
}

VertexBuffer* GraphicsDevice::CreateVertexBuffer(uint32_t vertexCount, uint32_t vertexSize, const void* initialData, bool dynamic)
{
	auto buffer = std::make_unique<VertexBuffer>(*m_device.Get(), vertexCount, vertexSize, initialData, dynamic);
	m_vertexBuffers.emplace_back(std::move(buffer));

	return m_vertexBuffers.back().get();
}

IndexBuffer* GraphicsDevice::CreateIndexBuffer(uint32_t indexCount, IndexSize indexSize, const void* initialData, bool dynamic)
{
	auto buffer = std::make_unique<IndexBuffer>(*m_device.Get(), indexCount, indexSize, initialData, dynamic);
	m_indexBuffers.emplace_back(std::move(buffer));

	return m_indexBuffers.back().get();
}

Texture* GraphicsDevice::CreateTexture(uint16_t width, uint16_t height, Format format, const void* data)
{
	auto buffer = std::make_unique<Texture>(*m_device.Get(), width, height, data, format);
	m_textures.emplace_back(std::move(buffer));

	return m_textures.back().get();
}
