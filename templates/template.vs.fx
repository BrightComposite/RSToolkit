/**
 *
 */

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT main(float4 position : POSITION)
{
	VS_OUTPUT output;
	output.position = position;
	
	return output;
}