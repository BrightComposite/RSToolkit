/**
 *	!layout: p2
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
};

VS_OUTPUT main(float2 position : POSITION)
{
	VS_OUTPUT output;
	output.position = float4(position * size + pos, 0.0, 1.0);
	return output;
}