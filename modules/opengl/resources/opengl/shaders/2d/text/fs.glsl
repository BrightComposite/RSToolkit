/**
 *
 */
#version 330 core

layout(std140) uniform BrushOptions
{
	vec4 color;
	float lineWidth;
};

uniform sampler2D tex;

in Vertex 
{
	vec2 texcoord;
} input;

out vec4 fscolor;

void main(void)
{
	fscolor = vec4(color[0], color[1], color[2], texture2D(tex, input.texcoord)[0] * color[3]);
}
