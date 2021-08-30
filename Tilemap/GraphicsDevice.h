#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "VertexShader.h"
#include "PixelShader.h"
#include "GraphicsContext.h"
#include "RenderWindow.h"
#include "ConstantBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class GraphicsDevice final
{
public:
	explicit GraphicsDevice();

public:
	RenderWindow* CreateRenderWindow(const TCHAR name[], uint16_t width, uint16_t height);

	VertexShader* CreateVertexShader(const void* data, const uint32_t dataSize);

	PixelShader* CreatePixelShader(const void* data, const uint32_t dataSize);

	ConstantBuffer* CreateConstantBuffer(uint32_t size, const void* initialData = nullptr);

	VertexBuffer* CreateVertexBuffer(uint32_t vertexCount, uint32_t vertexSize, const void* initialData = nullptr, bool dynamic = false);

	IndexBuffer* CreateIndexBuffer(uint32_t indexCount, IndexSize indexSize, const void* initialData = nullptr, bool dynamic = false);

	Texture* CreateTexture(uint16_t width, uint16_t height, Format format, const void* data);

public:
	GraphicsContext& GetImmediateContext() const { return *m_immediateContext; }

private:
	ComPtr<ID3D11Device> m_device;
	std::unique_ptr<GraphicsContext> m_immediateContext;
	std::vector<std::unique_ptr<VertexShader>> m_vertexShader;
	std::vector<std::unique_ptr<PixelShader>> m_pixelShader;
	std::vector<std::unique_ptr<RenderWindow>> m_renderWindows;
	std::vector<std::unique_ptr<ConstantBuffer>> m_constantBuffers;
	std::vector<std::unique_ptr<VertexBuffer>> m_vertexBuffers;
	std::vector<std::unique_ptr<IndexBuffer>> m_indexBuffers;
	std::vector<std::unique_ptr<Texture>> m_textures;
};