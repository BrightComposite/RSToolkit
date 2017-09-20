//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char * const shader_code_2d_ellipse_fs = R"SHADER(
/**
 *
 */
#version 330 core

layout(std140) uniform Color
{
	vec4 color;
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

)SHADER";

//---------------------------------------------------------------------------
