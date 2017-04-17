/**
 *
 */

Texture2D tex;
SamplerState state;

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_Target
{
    return tex.Sample(state, input.texcoord);
}