//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char shader_code_2d_wired_rect_fs = R"SHADER(
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
	vec2 ratio;
} input;

out vec4 fscolor;

void main(void)
{
	float lx = lineWidth / input.ratio.x;
	float ly = lineWidth / input.ratio.y;

	float x = input.texcoord[0];
	float y = input.texcoord[1];

	if(x > lx && x < (1.0 - lx) && y > ly && y < (1.0 - ly))
		discard;

	fscolor = color;
}

)SHADER";

//---------------------------------------------------------------------------
