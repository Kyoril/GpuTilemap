#pragma once

#include <cstdint>
#include <map>

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class RenderWindow;
class VertexShader;
class PixelShader;
class ConstantBuffer;
class VertexBuffer;
class IndexBuffer;
class Texture;

enum class Topology : uint8_t
{
	Undefined,

	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	LineListAdj,
	LineStripAdj,
	TriangleListAdj,
	TriangleStripAdj,
};

enum class ShaderStage : uint8_t
{
	VertexShader,
	PixelShader
};

/// Enumerates possible polygon fill modes.
enum class FillMode
{
	/// Polygons will be rendered solid.
	Solid,
	/// Only the edges of polygons will be rendered.
	Wireframe,
};

/// Enumerates possible face cull modes.
enum class FaceCullMode
{
	/// No front- or backface culling enabled.
	None,
	/// Back-facing polygons will be culled.
	Back,
	/// Front-facing polygons will be culled.
	Front,
};

/// Enumerates possible face cull modes.
enum class TextureAddressMode
{
	/// Coordinates are clamped if exceeding the range of 0..1.
	Clamp,
	/// Coordinates are wrapped if exceeding the range of 0..1.
	Wrap,
	/// Coordinates are mirrored if exceeding the range of 0..1.
	Mirror,
	/// Anything outside of the range of 0..1 is rendered using a border color.
	Border,
};

/// Enumerates possible face cull modes.
enum class TextureFilter
{
	/// No texture filtering.
	None,
	/// Bilinear filter.
	Bilinear,
	/// Trilinear filter.
	Trilinear,
	/// Anisotropic filter
	Anisotropic,
};

/// Enumerates possible blend modes.
enum class BlendMode
{
	/// Opaque rendering. Most performant, default mode.
	Opaque,
	/// Alpha blending enabled.
	Alpha,
};

class GraphicsContext final
{
public:
	explicit GraphicsContext(ID3D11Device& device, ComPtr<ID3D11DeviceContext>& context);

private:
	void CreateRasterizerState(bool set = false);

	void CreateBlendStates();

	void InitSamplerState();
	
	void UpdateCurrentRasterizerState();

public:
	void Reset();

	/// @brief Sets the viewport data.
	void SetViewport(int32_t left, int32_t top, int32_t width, int32_t height, float minDepth, float maxDepth);

	/// @brief Sets the active render target to render to.
	/// @param target The new active render target.
	void SetRenderTarget(RenderWindow& target);

	/// @brief Clears the given render target.
	/// @param target The render target to clear.
	void ClearRenderTarget(RenderWindow& target);

	void SetVertexShader(VertexShader& shader);

	void SetPixelShader(PixelShader& shader);

	void SetTopology(Topology topology);

	void SetConstantBuffer(ShaderStage stage, uint32_t bufferCount, ConstantBuffer* buffers[]);

	void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation = 0, uint32_t baseVertexLocation = 0);

	void SetVertexBuffer(VertexBuffer& buffer);

	void SetIndexBuffer(IndexBuffer& buffer);
	
	void SetClipRect(int32_t x, int32_t y, int32_t w, int32_t h);

	void ResetClipRect();

	void SetFillMode(FillMode mode);

	void SetFaceCullMode(FaceCullMode mode);

	void SetTextureAddressMode(TextureAddressMode mode);

	void SetTextureFilter(TextureFilter filter);
	
	void SetBlendMode(BlendMode mode);

	void SetTextures(ShaderStage stage, uint32_t numTextures, Texture** textures);

	void UpdateConstantBuffer(ConstantBuffer& buffer, const void* data);

private:
	ID3D11Device& m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	BlendMode m_blendMode = BlendMode::Alpha;
	FillMode m_fillMode = FillMode::Solid;
	FaceCullMode m_cullMode = FaceCullMode::None;
	TextureAddressMode m_texAddressMode = TextureAddressMode::Border;
	TextureFilter m_texFilter = TextureFilter::None;
	D3D11_RASTERIZER_DESC m_rasterizerDesc;
	bool m_rasterizerDescChanged = false;
	size_t m_rasterizerHash = 0;
	D3D11_SAMPLER_DESC m_samplerDesc;
	bool m_samplerDescChanged = false;
	size_t m_samplerHash = 0;
	std::map<size_t, ComPtr<ID3D11RasterizerState>> m_rasterizerStates;
	std::map<size_t, ComPtr<ID3D11SamplerState>> m_samplerStates;
	ComPtr<ID3D11BlendState> m_opaqueBlendState;
	ComPtr<ID3D11BlendState> m_alphaBlendState;
	ComPtr<ID3D11SamplerState> m_samplerState;
};
