/**
 *	!vertex: p3
 */
#version 330 core

layout(std140) uniform Model
{
	mat4 model;
};

layout(std140) uniform View
{
	mat4 view;
};

layout(std140) uniform Projection
{
	mat4 projection;
};

in vec3 position;

void main(void)
{
    gl_Position = projection * view * model * vec4(position, 1.0);
}
