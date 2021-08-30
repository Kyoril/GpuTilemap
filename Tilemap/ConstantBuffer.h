#pragma once

#include <cstdint>

#include "BufferBase.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

interface ID3D11Device;
interface ID3D11Buffer;

class ConstantBuffer final : public BufferBase
{
	friend class GraphicsContext;

public:
	explicit ConstantBuffer(ID3D11Device& device, uint32_t size, const void* initialData = nullptr);
	~ConstantBuffer() override = default;

public:
	void* Map() override;
	void Unmap() override;

private:
	ComPtr<ID3D11Buffer> m_buffer;
};