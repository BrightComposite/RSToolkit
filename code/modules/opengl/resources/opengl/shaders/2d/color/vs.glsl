/**
 *	!vertex: p2 c3
 */
#version 330 core

in vec2 position;
in vec3 color;

out Vertex 
{
	vec4 color;
} vtx;

void main(void)
{
	vtx.color = vec4(color, 1.0);
	gl_Position = vec4(position, 0.0, 1.0);
}
