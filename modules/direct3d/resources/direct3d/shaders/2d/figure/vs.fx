/**
*
*/

cbuffer Model : register(b1)
{
	float4x4 model;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT main(float2 position : POSITION)
{
	VS_OUTPUT output;
	output.position = mul(float4(position, 0.0, 1.0), model);
	return output;
}