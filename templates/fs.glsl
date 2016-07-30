/**
 *
 */
#version 330 core

layout(std140) uniform BrushOptions
{
	vec4 color;
	float lineWidth;
};

out vec4 fscolor;

void main(void)
{
	fscolor = color;
}