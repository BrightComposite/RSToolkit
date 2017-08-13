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

layout(std140) uniform ImageScale
{
	vec2 scale;
};

in vec2 position;
in vec2 texcoord;

out Vertex 
{
	vec2 texcoord;
} output;

void main(void)
{
	output.texcoord = texcoord * scale;
    gl_Position = vec4(position * size + pos, depth, 1.0);
}
