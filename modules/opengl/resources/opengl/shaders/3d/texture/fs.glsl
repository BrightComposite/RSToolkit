/**
 *
 */
#version 330 core

uniform sampler2D texture0;

in Vertex 
{
	vec2 texcoord;
} input;

out vec4 fscolor;

void main(void)
{
	fscolor = texture2D(texture0, input.texcoord);
}
