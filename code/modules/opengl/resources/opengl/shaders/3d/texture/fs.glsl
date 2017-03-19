/**
 *
 */
#version 330 core

uniform sampler2D texture0;

in Vertex 
{
	vec2 texcoord;
} vtx;

out vec4 fscolor;

void main(void)
{
	fscolor = texture2D(texture0, vtx.texcoord);
}
