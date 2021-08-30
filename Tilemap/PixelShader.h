#pragma once

#include <cstdint>

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class PixelShader final
{
	friend class GraphicsContext;

public:
	explicit PixelShader(ID3D11Device& device, const void* data, uint32_t dataSize);

private:
	ComPtr<ID3D11PixelShader> m_pixelShader;
};
