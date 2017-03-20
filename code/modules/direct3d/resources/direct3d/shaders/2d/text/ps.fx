/**
 *
 */

cbuffer BrushOptions : register(b0)
{
	float4 color;
	float lineWidth;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

Texture2D tex;
SamplerState state;

float4 main(VS_OUTPUT input) : SV_Target
{
    return float4(color[0], color[1], color[2], tex.Sample(state, input.texcoord)[0] * color[3]);
}