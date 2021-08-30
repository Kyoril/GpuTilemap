
#include "ConstantBuffer.h"

#include <d3d11.h>

#include "ComUtils.h"

ConstantBuffer::ConstantBuffer(ID3D11Device& device, const uint32_t size, const void* initialData)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = size;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA bufferData;
	if (initialData)
	{
		ZeroMemory(&bufferData, sizeof(bufferData));
		bufferData.pSysMem = initialData;
	}

	HRESULT hr = device.CreateBuffer(&bufferDesc, initialData ? &bufferData : nullptr, &m_buffer);
	ThrowIfFail(hr);
}
