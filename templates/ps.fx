/**
 *
 */

cbuffer Color : register(b0)
{
	float4 color;
	float lineWidth;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

float4 main(VS_OUTPUT input) : SV_Target
{
    return color;
}