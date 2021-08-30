struct VS_Out
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;

	float2 pixelCoord : TEXCOORD1;
	float2 texCoord : TEXCOORD2;
};

cbuffer PsPerObject
{
	float2 inverseTilesetTextureSize;
	float2 inverseTilemapTextureSize;
	float tileSize;
};


Texture2D tileset : register(t0);
Texture2D tilemap : register(t1);
SamplerState texSampler;

float2 mod(float2 x, float y)
{
	return x - y * floor(x/y);
}

float4 main(VS_Out input) : SV_TARGET
{
	float4 tile = tilemap.Sample(texSampler, input.texCoord);
	if (tile.x == 1.0 && tile.y == 1.0) 
	{
		discard;
	}

	float2 spriteOffset = floor(tile.xy * 256.0) * tileSize;
	float2 spriteCoord = mod(input.pixelCoord, tileSize);

	return tileset.Sample(texSampler, (spriteOffset + spriteCoord) * inverseTilesetTextureSize);
}