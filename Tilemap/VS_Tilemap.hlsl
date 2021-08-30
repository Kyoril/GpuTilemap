	
#pragma pack_matrix( row_major )

cbuffer VsPerObject
{
	matrix matWorld;
	matrix matView;
	matrix matProj;
};

cbuffer VsPerFrame
{
	float2 viewOffset;
	float2 viewportSize;
	float2 inverseTileTextureSize;
	float inverseTileSize;
};

struct VS_In
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
	uint id : VERTEXID;
};

struct VS_Out
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;

	float2 pixelCoord : TEXCOORD1;
	float2 texCoord : TEXCOORD2;
};

VS_Out main( VS_In input )
{
	VS_Out output;
	
	if (input.id == 1) input.pos.x = viewportSize.x * 2.0;
	else if (input.id == 2) input.pos.y = viewportSize.y * 2.0;
	else if (input.id == 3) 
	{
		input.pos.x = viewportSize.x * 2.0;
		input.pos.y = viewportSize.y * 2.0;
	}

	output.pixelCoord = (input.uv * viewportSize) + viewOffset;
	output.texCoord = output.pixelCoord * inverseTileTextureSize * inverseTileSize;

	input.pos.w = 1.0f;
	output.pos = mul(input.pos, matWorld);
	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);
	
	output.uv = input.uv;

	return output;
}