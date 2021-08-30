
#include "GraphicsContext.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "RenderWindow.h"
#include "ConstantBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "ComUtils.h"
#include "DynamicHash.h"
#include "Texture.h"

#include <cassert>

GraphicsContext::GraphicsContext(ID3D11Device& device, ComPtr<ID3D11DeviceContext>& context)
	: m_device(device)
	, m_context(context)
{
	CreateBlendStates();
	InitSamplerState();
}

namespace
{
	static D3D11_FILL_MODE D3D11FillMode(FillMode mode)
	{
		switch (mode)
		{
		case FillMode::Wireframe:
			return D3D11_FILL_WIREFRAME;
		}

		return D3D11_FILL_SOLID;
	}

	static D3D11_CULL_MODE D3D11CullMode(FaceCullMode mode)
	{
		switch (mode)
		{
		case FaceCullMode::Back:
			return D3D11_CULL_BACK;
		case FaceCullMode::Front:
			return D3D11_CULL_FRONT;
		}

		return D3D11_CULL_NONE;
	}

	static D3D11_TEXTURE_ADDRESS_MODE D3D11TextureAddressMode(TextureAddressMode mode)
	{
		switch (mode)
		{
		case TextureAddressMode::Clamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case TextureAddressMode::Wrap:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case TextureAddressMode::Border:
			return D3D11_TEXTURE_ADDRESS_BORDER;
		case TextureAddressMode::Mirror:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		}

		return D3D11_TEXTURE_ADDRESS_CLAMP;
	}

	static D3D11_FILTER D3D11TextureFilter(TextureFilter mode)
	{
		switch (mode)
		{
		case TextureFilter::None:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case TextureFilter::Bilinear:
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case TextureFilter::Trilinear:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case TextureFilter::Anisotropic:
			return D3D11_FILTER_ANISOTROPIC;
		}

		return D3D11_FILTER_ANISOTROPIC;
	}
}

void GraphicsContext::InitSamplerState()
{
	ZeroMemory(&m_samplerDesc, sizeof(m_samplerDesc));
	m_samplerDesc.Filter = D3D11TextureFilter(m_texFilter);
	m_samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
	m_samplerDesc.AddressU = D3D11TextureAddressMode(m_texAddressMode);
	m_samplerDesc.AddressV = D3D11TextureAddressMode(m_texAddressMode);
	m_samplerDesc.AddressW = D3D11TextureAddressMode(m_texAddressMode);
	m_samplerDesc.BorderColor[0] = 1.0f;
	m_samplerDesc.BorderColor[1] = 1.0f;
	m_samplerDesc.BorderColor[2] = 1.0f;
	m_samplerDesc.BorderColor[3] = 0.0f;
	m_samplerDescChanged = true;

	HRESULT hr = m_device.CreateSamplerState(&m_samplerDesc, &m_samplerState);
	ThrowIfFail(hr);
}

void GraphicsContext::Reset()
{
	m_context->ClearState();
}

/// @brief Sets the viewport data.
void GraphicsContext::SetViewport(int32_t left, int32_t top, int32_t width, int32_t height, float minDepth, float maxDepth)
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = static_cast<float>(left);
	vp.TopLeftY = static_cast<float>(top);
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = minDepth;
	vp.MaxDepth = maxDepth;
	m_context->RSSetViewports(1, &vp);
}

/// @brief Sets the active render target to render to.
/// @param target The new active render target.
void GraphicsContext::SetRenderTarget(RenderWindow& target)
{
	m_context->OMSetRenderTargets(1, target.m_renderTargetView.GetAddressOf(), nullptr);
}

/// @brief Clears the given render target.
/// @param target The render target to clear.
void GraphicsContext::ClearRenderTarget(RenderWindow& target)
{
	// TODO: Clear color from render target
	const FLOAT clearColor[] = { 0.0f, 0.2f, 0.4f };
	m_context->ClearRenderTargetView(target.m_renderTargetView.Get(), clearColor);
}

void GraphicsContext::SetVertexShader(VertexShader& shader)
{
	m_context->VSSetShader(shader.m_vertexShader.Get(), nullptr, 0);
	m_context->IASetInputLayout(shader.m_inputLayout.Get());
}

void GraphicsContext::SetPixelShader(PixelShader& shader)
{
	m_context->PSSetShader(shader.m_pixelShader.Get(), nullptr, 0);
}

namespace
{
	inline D3D11_PRIMITIVE_TOPOLOGY MapTopology(Topology topology)
	{
		switch(topology)
		{
		case Topology::Undefined:		return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case Topology::PointList:		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case Topology::LineList:		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case Topology::LineStrip:		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case Topology::TriangleList:		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case Topology::TriangleStrip:		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case Topology::LineListAdj:		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
		case Topology::LineStripAdj:		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
		case Topology::TriangleListAdj:		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
		case Topology::TriangleStripAdj:		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
		default:
			return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	}
}

void GraphicsContext::SetTopology(Topology topology)
{
	m_context->IASetPrimitiveTopology(MapTopology(topology));
}

void GraphicsContext::SetConstantBuffer(ShaderStage stage, const uint32_t bufferCount, ConstantBuffer* buffers[])
{
	assert(bufferCount <= D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
	ID3D11Buffer* d3dBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};

	for (int i = 0; i < bufferCount; ++i)
	{
		d3dBuffers[i] = buffers[i]->m_buffer.Get();
	}

	switch(stage)
	{
	case ShaderStage::VertexShader:
		m_context->VSSetConstantBuffers(0, bufferCount, d3dBuffers);
		break;
	case ShaderStage::PixelShader:
		m_context->PSSetConstantBuffers(0, bufferCount, d3dBuffers);
		break;
	default:
		assert(false);
	}
}

class RasterizerStateHash final
{
public:
	size_t operator()(const D3D11_RASTERIZER_DESC& desc) const 
	{
		DynamicHash hash;
		hash.Add64(desc.AntialiasedLineEnable);
		hash.Add64(desc.CullMode);
		hash.Add64(desc.DepthBias);
		hash.AddFloat(desc.DepthBiasClamp);
		hash.Add64(desc.DepthClipEnable);
		hash.Add64(desc.FillMode);
		hash.Add64(desc.FrontCounterClockwise);
		hash.Add64(desc.MultisampleEnable);
		hash.Add64(desc.ScissorEnable);
		hash.AddFloat(desc.SlopeScaledDepthBias);
		return hash;
	}
};

void GraphicsContext::UpdateCurrentRasterizerState()
{
	// Need rasterizer state update?
	if (m_rasterizerDescChanged)
	{
		// Calculate the current hash
		const RasterizerStateHash hashGen;
		m_rasterizerHash = hashGen(m_rasterizerDesc);

		m_rasterizerDescChanged = false;
	}
		
	// Check if rasterizer state for this hash has already been created
	auto it = m_rasterizerStates.find(m_rasterizerHash);
	if (it == m_rasterizerStates.end())
	{
		// Not yet created, so create and activate it
		CreateRasterizerState(true);
	}
	else
	{
		// State exists, so activate it
		m_context->RSSetState(it->second.Get());
	}
}

void GraphicsContext::CreateRasterizerState(bool set)
{
	// Create hash generator
	const RasterizerStateHash hashGen;

	// Generate hash from rasterizer desc
	m_rasterizerHash = hashGen(m_rasterizerDesc);

	ComPtr<ID3D11RasterizerState> state;
	HRESULT hr = m_device.CreateRasterizerState(&m_rasterizerDesc, state.GetAddressOf());
	m_rasterizerStates[m_rasterizerHash] = state;

	// Activate new state if requested
	if (set)
	{
		m_context->RSSetState(state.Get());
	}
}

void GraphicsContext::CreateBlendStates()
{
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT hr = m_device.CreateBlendState(&bd, &m_opaqueBlendState);
	ThrowIfFail(hr);

	// Create the blend state
	ZeroMemory(&bd, sizeof(bd));
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device.CreateBlendState(&bd, &m_alphaBlendState);
	ThrowIfFail(hr);
}

void GraphicsContext::DrawIndexed(const uint32_t indexCount, const uint32_t startIndexLocation, const uint32_t baseVertexLocation)
{
	UpdateCurrentRasterizerState();

	assert(indexCount > 0);
	m_context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void GraphicsContext::SetVertexBuffer(VertexBuffer& buffer)
{
	UINT stride = buffer.GetVertexSize();
	UINT offset = 0;
	m_context->IASetVertexBuffers(0, 1, buffer.m_buffer.GetAddressOf(), &stride, &offset);
}

void GraphicsContext::SetIndexBuffer(IndexBuffer& buffer)
{
	m_context->IASetIndexBuffer(buffer.m_buffer.Get(), buffer.GetIndexSize() == IndexSize::Index16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
}

void GraphicsContext::SetClipRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
	D3D11_RECT clipRect;
	clipRect.left = x;
	clipRect.top = y;
	clipRect.right = x + w;
	clipRect.bottom = y + h;
	m_context->RSSetScissorRects(1, &clipRect);

	m_rasterizerDesc.ScissorEnable = TRUE;
	m_rasterizerDescChanged = true;
}

void GraphicsContext::ResetClipRect()
{
	m_rasterizerDesc.ScissorEnable = FALSE;
	m_rasterizerDescChanged = true;
}

void GraphicsContext::SetFillMode(FillMode mode)
{
	m_fillMode = mode;
	m_rasterizerDesc.FillMode = D3D11FillMode(mode);
	m_rasterizerDescChanged = true;
}

void GraphicsContext::SetFaceCullMode(FaceCullMode mode)
{
	m_cullMode = mode;
	m_rasterizerDesc.CullMode = D3D11CullMode(mode);
	m_rasterizerDescChanged = true;
}

void GraphicsContext::SetTextureAddressMode(TextureAddressMode mode)
{
	m_samplerDesc.AddressU = D3D11TextureAddressMode(mode);
	m_samplerDesc.AddressV = D3D11TextureAddressMode(mode);
	m_samplerDesc.AddressW = D3D11TextureAddressMode(mode);
	m_samplerDescChanged = true;
}

void GraphicsContext::SetTextureFilter(TextureFilter filter)
{
	m_samplerDesc.Filter = D3D11TextureFilter(filter);
	m_samplerDescChanged = true;
}

void GraphicsContext::SetBlendMode(BlendMode mode)
{
	m_blendMode = mode;

	ID3D11BlendState* blendState = nullptr;
	switch (mode)
	{
	case BlendMode::Opaque:
		blendState = m_opaqueBlendState.Get();
		break;
	case BlendMode::Alpha:
		blendState = m_alphaBlendState.Get();
		break;
	}

	assert(blendState);
	m_context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
}

void GraphicsContext::SetTextures(ShaderStage stage, uint32_t numTextures, Texture** textures)
{
	ID3D11ShaderResourceView* views[8] = {
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};

	ID3D11SamplerState* states[8] = {
		m_samplerState.Get(), m_samplerState.Get(), m_samplerState.Get(), m_samplerState.Get(), 
		m_samplerState.Get(), m_samplerState.Get(), m_samplerState.Get(), m_samplerState.Get()
	};

	for (uint32_t i = 0; i < numTextures; ++i)
	{
		views[i] = textures[i]->m_shaderResView.Get();
	}

	switch(stage)
	{
	case ShaderStage::VertexShader:
		m_context->VSSetShaderResources(0, numTextures, views);
		m_context->VSSetSamplers(0, numTextures, states);
		break;
	case ShaderStage::PixelShader:
		m_context->PSSetShaderResources(0, numTextures, views);
		m_context->PSSetSamplers(0, numTextures, states);
		break;
	}
}

void GraphicsContext::UpdateConstantBuffer(ConstantBuffer& buffer, const void* data)
{
	m_context->UpdateSubresource(buffer.m_buffer.Get(), 0, nullptr, data, 0, 0);
}
