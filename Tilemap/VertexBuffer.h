#pragma once

#include "BufferBase.h"

#include <cstdint>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11Buffer;

class VertexBuffer final : public BufferBase
{
	friend class GraphicsContext;

public:
	explicit VertexBuffer(ID3D11Device& device, uint32_t vertexCount, uint32_t vertexSize, const void* initialData = nullptr, bool dynamic = false);
	~VertexBuffer() override = default;

public:
	uint32_t GetVertexCount() const { return m_vertexCount; }
	uint32_t GetVertexSize() const { return m_vertexSize; }
	bool IsDynamic() const { return m_dynamic; }

private:
	uint32_t m_vertexCount;
	uint32_t m_vertexSize;
	bool m_dynamic;
	ComPtr<ID3D11Buffer> m_buffer;
};