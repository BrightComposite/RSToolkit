/**
 *	!layout: p2 t
 */

cbuffer Area : register(b4)
{
	float2 pos;
	float2 size;
	float  depth;
};

cbuffer Viewport : register(b5)
{
	float2 viewport;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 ratio    : COLOR;
};

VS_OUTPUT main(float2 position : POSITION, float2 texcoord : TEXCOORD)
{
	VS_OUTPUT output;

	output.ratio.w = output.ratio.z = max(size.x, size.y);
	output.ratio.xy = size / output.ratio.wz;
	output.ratio.w *= viewport.x * 0.5;

	output.position = float4((position * output.ratio.z + pos) * float2(1.0, viewport.x / viewport.y), depth, 1.0);
	output.texcoord = texcoord;

	return output;
}