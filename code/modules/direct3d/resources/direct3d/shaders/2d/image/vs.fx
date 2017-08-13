/**
 *	!layout: p2 t
 */

cbuffer Area : register(b4)
{
	float2 pos;
	float2 size;
	float  depth;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

VS_OUTPUT main(float2 position : POSITION, float2 texcoord : TEXCOORD)
{
	VS_OUTPUT output;

	output.position = float4(position * size + pos, depth, 1.0);
	output.texcoord = texcoord;

	return output;
}