/**
 *
 */
#version 330 core

layout(std140) uniform BrushOptions
{
	vec4 color;
	float lineWidth;
};

in Vertex 
{
	vec2 texcoord;
	vec4 ratio;
} input;

out vec4 fscolor;

void main(void)
{
	float x = (input.texcoord[0] * 2.0 - 1.0) * input.ratio.x;
	float y = (input.texcoord[1] * 2.0 - 1.0) * input.ratio.y;

	if(x * x + y * y > 1.0)
		discard;

	fscolor = color;
}
