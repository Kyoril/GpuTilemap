
#include "VertexShader.h"
#include "ComUtils.h"

VertexShader::VertexShader(ID3D11Device& device, const void* data, const uint32_t dataSize)
{
	HRESULT hr = device.CreateVertexShader(data, dataSize, nullptr, &m_vertexShader);
	ThrowIfFail(hr);

	const D3D11_INPUT_ELEMENT_DESC PosElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VERTEXID", 0, DXGI_FORMAT_R32_UINT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = device.CreateInputLayout(PosElements, ARRAYSIZE(PosElements), data, dataSize, &m_inputLayout);
	ThrowIfFail(hr);
}
