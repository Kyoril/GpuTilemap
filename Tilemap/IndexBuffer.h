#pragma once

#include <cstdint>

#include "BufferBase.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11Buffer;

enum class IndexSize : uint8_t 
{
	Index16,
	Index32
};

class IndexBuffer final : public BufferBase
{
	friend class GraphicsContext;

public:
	explicit IndexBuffer(ID3D11Device& device, uint32_t indexCount, IndexSize indexSize, const void* initialData = nullptr, bool dynamic = false);
	~IndexBuffer() override = default;

public:
	uint32_t GetIndexCount() const { return m_indexCount; }
	IndexSize GetIndexSize() const { return m_indexSize; }
	bool IsDynamic() const { return m_dynamic; }

private:
	uint32_t m_indexCount;
	IndexSize m_indexSize;
	bool m_dynamic;
	ComPtr<ID3D11Buffer> m_buffer;
};