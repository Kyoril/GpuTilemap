
#include "PixelShader.h"
#include "ComUtils.h"

PixelShader::PixelShader(ID3D11Device& device, const void* data, const uint32_t dataSize)
{
	HRESULT hr = device.CreatePixelShader(data, dataSize, nullptr, &m_pixelShader);
	ThrowIfFail(hr);
}
