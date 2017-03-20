/**
 *	!vertex: p2 t
 */
#version 330 core

layout(std140) uniform Area
{
	vec2  pos;
	vec2  size;
	float depth;
};

layout(std140) uniform Viewport
{
	vec2 viewport;
};

in vec2 position;
in vec2 texcoord;

out Vertex 
{
	vec2 texcoord;
	vec4 ratio;
} output;

void main(void)
{
	output.ratio.w = output.ratio.z = max(size.x, size.y);
	output.ratio.xy = size / output.ratio.wz;
	output.ratio.w *= viewport.x * 0.5;
	
	output.texcoord = texcoord;

	gl_Position = vec4((position * output.ratio.z + pos) * vec2(1.0, viewport.x / viewport.y), depth, 1.0);
}
