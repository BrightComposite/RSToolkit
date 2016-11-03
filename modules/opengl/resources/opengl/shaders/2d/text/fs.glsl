/**
 *
 */
#version 330 core

layout(std140) uniform Color
{
	vec4 color;
};

uniform sampler2D texture0;

in Vertex 
{
	vec2 texcoord;
} input;

out vec4 fscolor;

void main(void)
{
	fscolor = vec4(color[0], color[1], color[2], texture2D(texture0, input.texcoord)[0] * color[3]);
}
