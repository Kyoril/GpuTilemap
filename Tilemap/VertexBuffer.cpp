
#include "VertexBuffer.h"

#include <d3d11.h>

#include <cassert>

#include "ComUtils.h"

VertexBuffer::VertexBuffer(ID3D11Device& device, const uint32_t vertexCount, const uint32_t vertexSize, const void* initialData, const bool dynamic)
	: m_vertexCount(vertexCount)
	, m_vertexSize(vertexSize)
	, m_dynamic(dynamic)
{
	assert(vertexCount > 0);
	assert(vertexSize > 0);

	// Allocate vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = m_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = static_cast<UINT>(m_vertexSize * m_vertexCount);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
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
