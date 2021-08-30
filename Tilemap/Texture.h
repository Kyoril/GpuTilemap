#pragma once

#include <cstdint>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

enum class Format : uint8_t
{
	R8G8B8A8,
	B8G8R8A8,
	B8G8R8X8,
};

class Texture final
{
	friend class GraphicsContext;

public:
	Texture(ID3D11Device& device, uint16_t width, uint16_t height, const void* data, Format format);

public:
	uint16_t GetWidth() const { return m_width; }
	uint16_t GetHeight() const { return m_height; }
	Format GetFormat() const { return m_format; }

private:
	uint16_t m_width;
	uint16_t m_height;
	Format m_format;
	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11ShaderResourceView> m_shaderResView;
};
