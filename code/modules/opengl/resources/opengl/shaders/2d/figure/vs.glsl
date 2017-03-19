/**
 *	!vertex: p2
 */
#version 330 core

layout(std140) uniform Model
{
	mat4 model;
};

layout(std140) uniform Projection
{
	mat4 projection;
};

in vec2 position;

void main(void)
{
    gl_Position = vec4(position, 0.0, 1.0) * model * projection;
}
