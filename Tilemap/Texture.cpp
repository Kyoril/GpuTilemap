
#include "Texture.h"
#include "ComUtils.h"

#include <d3d11.h>

namespace
{
	inline DXGI_FORMAT MapFormat(Format format)
	{
		switch(format)
		{
		case Format::R8G8B8A8:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::B8G8R8A8:		return DXGI_FORMAT_B8G8R8A8_UNORM;
		case Format::B8G8R8X8:		return DXGI_FORMAT_B8G8R8X8_UNORM;
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	inline uint32_t GetNumChannels(Format format)
	{
		switch(format)
		{
		case Format::R8G8B8A8:		return 4;
		case Format::B8G8R8A8:		return 4;
		case Format::B8G8R8X8:		return 3;
		}

		return 0;
	}
}

Texture::Texture(ID3D11Device& device, uint16_t width, uint16_t height, const void* data, Format format)
	: m_width(width)
	, m_height(height)
	, m_format(format)
{
	// Create texture description
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.ArraySize = 1;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.Height = height;
	td.Width = width;
	td.MipLevels = 1;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_IMMUTABLE;
	td.Format = MapFormat(format);
	
	// Prepare usage data
	D3D11_SUBRESOURCE_DATA id;
	ZeroMemory(&id, sizeof(id));
	id.SysMemPitch = sizeof(uint8_t) * GetNumChannels(format) * width;
	id.pSysMem = data;

	HRESULT hr = device.CreateTexture2D(&td, &id, &m_texture);
	ThrowIfFail(hr);

	// Create shader resource view description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;

	// Create shader resource view
	hr = device.CreateShaderResourceView(m_texture.Get(), &srvd, &m_shaderResView);
	ThrowIfFail(hr);
}
