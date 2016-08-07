/**
 *
 */
#version 330 core

in Vertex 
{
	vec4 color;
} input;

out vec4 fscolor;

void main(void)
{
	fscolor = input.color;
}
