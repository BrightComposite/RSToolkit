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
	float4 ratio : NORMAL;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float x = (input.texcoord[0] * 2.0 - 1.0) / input.ratio.x;
	float y = (input.texcoord[1] * 2.0 - 1.0) / input.ratio.y;

	if(x * x + y * y > 1.0)
		clip(-1);

	return color;
}