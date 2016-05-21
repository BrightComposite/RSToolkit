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
	float l = lineWidth / input.ratio.w;

	float x = input.texcoord[0] * 2.0 - 1.0;
	float y = input.texcoord[1] * 2.0 - 1.0;

	float x1 = x / input.ratio.x;
	float y1 = y / input.ratio.y;
	float x2 = x / (input.ratio.x - l);
	float y2 = y / (input.ratio.y - l);

	if(x1 * x1 + y1 * y1 >= 1.0 || x2 * x2 + y2 * y2 < 1.0)
		clip(-1);

	return color;
}