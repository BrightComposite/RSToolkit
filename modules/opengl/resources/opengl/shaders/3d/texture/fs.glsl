/**
 *
 */
#version 330 core

uniform sampler2D tex;

in Vertex 
{
	vec2 texcoord;
} input;

out vec4 fscolor;

void main(void)
{
	fscolor = texture2D(tex, input.texcoord);
}
