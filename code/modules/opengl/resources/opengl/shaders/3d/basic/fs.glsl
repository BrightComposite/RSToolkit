/**
 *
 */
#version 330 core

layout(std140) uniform Color
{
	vec4 color;
};

out vec4 fscolor;

void main(void)
{
	fscolor = color;
}
