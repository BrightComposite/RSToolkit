//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char shader_code_2d_wired_ellipse_fs = R"SHADER(
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
} vtx;

out vec4 fscolor;

void main(void)
{
	float l = lineWidth / vtx.ratio.w;

	float x = vtx.texcoord[0] * 2.0 - 1.0;
	float y = vtx.texcoord[1] * 2.0 - 1.0;

	float x1 = x / vtx.ratio.x;
	float y1 = y / vtx.ratio.y;
	float x2 = x / (vtx.ratio.x - l);
	float y2 = y / (vtx.ratio.y - l);

	if(x1 * x1 + y1 * y1 >= 1.0 || x2 * x2 + y2 * y2 < 1.0)
		discard;

	fscolor = color;
}

)SHADER";

//---------------------------------------------------------------------------
