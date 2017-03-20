/**
 *
 */
#version 330 core

layout(std140) uniform BrushOptions
{
	vec4  color;
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
	float l = lineWidth / input.ratio.w;

	float x = input.texcoord[0] * 2.0 - 1.0;
	float y = input.texcoord[1] * 2.0 - 1.0;

	float x1 = x / input.ratio.x;
	float y1 = y / input.ratio.y;
	float x2 = x / (input.ratio.x - l);
	float y2 = y / (input.ratio.y - l);

	if(x1 * x1 + y1 * y1 >= 1.0 || x2 * x2 + y2 * y2 < 1.0)
		discard;

	fscolor = color;
}
