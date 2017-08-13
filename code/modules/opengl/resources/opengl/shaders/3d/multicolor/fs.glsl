/**
 *
 */
#version 330 core

in Vertex 
{
	vec4 color;
} vtx;

out vec4 fscolor;

void main(void)
{
	fscolor = vtx.color;
}
