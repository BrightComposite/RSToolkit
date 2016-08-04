/**
 *	!vertex: p2
 */
#version 330 core

layout(std140) uniform Area
{
	vec2  pos;
	vec2  size;
	float depth;
};

layout(location = 0) in vec2 position;

void main(void)
{
    gl_Position = vec4(position * size + pos, depth, 1.0);
}
