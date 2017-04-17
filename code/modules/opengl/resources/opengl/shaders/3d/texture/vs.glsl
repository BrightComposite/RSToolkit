/**
 *	!vertex: p3 t
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
in vec2 texcoord;

out Vertex 
{
	vec2 texcoord;
} output;

void main(void)
{
    output.texcoord = texcoord;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
