/**
*
*/

cbuffer Model : register(b1)
{
	float4x4 model;
};

cbuffer View : register(b2)
{
	float4x4 view;
};

cbuffer Projection : register(b3)
{
	float4x4 projection;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT main(float3 position : POSITION, float4 color : COLOR)
{
	VS_OUTPUT output;

	output.position = mul(mul(mul(float4(position, 1.0), model), view), projection);
	output.color = color;

	return output;
}
