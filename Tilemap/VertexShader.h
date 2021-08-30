#pragma once

#include <cstdint>

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class VertexShader final
{
	friend class GraphicsContext;

public:
	explicit VertexShader(ID3D11Device& device, const void* data, const uint32_t dataSize);

private:
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11InputLayout> m_inputLayout;
};