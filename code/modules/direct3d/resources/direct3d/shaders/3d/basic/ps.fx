/**
 *
 */

cbuffer Animation : register(b0)
{
	float degree;
};

float4 main(float4 pos : SV_POSITION) : SV_Target
{
	const float limiter = 1920.0f;

	pos.x -= 960.0f;
	pos.y -= 540.0f;

	float4 dist11 = float4(
		(pos.x + 0.00) * (pos.x + 0.00) + (pos.y + 0.00) * (pos.y + 0.00),
		(pos.x + 0.25) * (pos.x + 0.25) + (pos.y + 0.00) * (pos.y + 0.00),
		(pos.x + 0.25) * (pos.x + 0.25) + (pos.y + 0.25) * (pos.y + 0.25),
		(pos.x + 0.00) * (pos.x + 0.00) + (pos.y + 0.25) * (pos.y + 0.25)
	);

	float4 dist12 = float4(
		(pos.x + 0.50) * (pos.x + 0.50) + (pos.y + 0.00) * (pos.y + 0.00),
		(pos.x + 0.75) * (pos.x + 0.75) + (pos.y + 0.00) * (pos.y + 0.00),
		(pos.x + 0.75) * (pos.x + 0.75) + (pos.y + 0.25) * (pos.y + 0.25),
		(pos.x + 0.50) * (pos.x + 0.50) + (pos.y + 0.25) * (pos.y + 0.25)
	);

	float4 dist21 = float4(
		(pos.x + 0.00) * (pos.x + 0.00) + (pos.y + 0.50) * (pos.y + 0.50),
		(pos.x + 0.25) * (pos.x + 0.25) + (pos.y + 0.50) * (pos.y + 0.50),
		(pos.x + 0.25) * (pos.x + 0.25) + (pos.y + 0.75) * (pos.y + 0.75),
		(pos.x + 0.00) * (pos.x + 0.00) + (pos.y + 0.75) * (pos.y + 0.75)
	);

	float4 dist22 = float4(
		(pos.x + 0.50) * (pos.x + 0.50) + (pos.y + 0.50) * (pos.y + 0.50),
		(pos.x + 0.75) * (pos.x + 0.75) + (pos.y + 0.50) * (pos.y + 0.50),
		(pos.x + 0.75) * (pos.x + 0.75) + (pos.y + 0.75) * (pos.y + 0.75),
		(pos.x + 0.50) * (pos.x + 0.50) + (pos.y + 0.75) * (pos.y + 0.75)
	);
	
	dist11[0] = (dist11[0] % limiter) / limiter;
	dist11[1] = (dist11[1] % limiter) / limiter;
	dist11[2] = (dist11[2] % limiter) / limiter;
	dist11[3] = (dist11[3] % limiter) / limiter;

	dist12[0] = (dist12[0] % limiter) / limiter;
	dist12[1] = (dist12[1] % limiter) / limiter;
	dist12[2] = (dist12[2] % limiter) / limiter;
	dist12[3] = (dist12[3] % limiter) / limiter;

	dist21[0] = (dist21[0] % limiter) / limiter;
	dist21[1] = (dist21[1] % limiter) / limiter;
	dist21[2] = (dist21[2] % limiter) / limiter;
	dist21[3] = (dist21[3] % limiter) / limiter;

	dist22[0] = (dist22[0] % limiter) / limiter;
	dist22[1] = (dist22[1] % limiter) / limiter;
	dist22[2] = (dist22[2] % limiter) / limiter;
	dist22[3] = (dist22[3] % limiter) / limiter;
	
	float d11 = (dist11[0] + dist11[1] + dist11[2] + dist11[3]) / 4;
	float d12 = (dist12[0] + dist12[1] + dist12[2] + dist12[3]) / 4;
	float d21 = (dist21[0] + dist21[1] + dist21[2] + dist21[3]) / 4;
	float d22 = (dist22[0] + dist22[1] + dist22[2] + dist22[3]) / 4;

	float dist = 1.0f - sin((d11 + d12 + d21 + d22) / 4 * 3.14);

    return float4(dist, dist, dist, 1.0f);
}