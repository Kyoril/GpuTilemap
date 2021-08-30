
#include "IndexBuffer.h"

#include <cassert>

#include <d3d11.h>
#include "ComUtils.h"

IndexBuffer::IndexBuffer(ID3D11Device& device, uint32_t indexCount, IndexSize indexSize, const void* initialData, bool dynamic)
	: m_indexCount(indexCount)
	, m_indexSize(indexSize)
	, m_dynamic(dynamic)
{
	assert(indexCount > 0);

	// Allocate vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = m_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = static_cast<UINT>(indexCount * (indexSize == IndexSize::Index16 ? sizeof(uint16_t) : sizeof(uint32_t)));
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = m_dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;

	// Fill buffer with initial data on creation to speed things up
	D3D11_SUBRESOURCE_DATA initData{};
	if (initialData)
	{
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = initialData;
	}

	HRESULT hr = device.CreateBuffer(&bufferDesc, initialData ? &initData : nullptr, &m_buffer);
	ThrowIfFail(hr);
}
