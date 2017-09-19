//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char shader_code_2d_figure_fs = R"SHADER(
/**
 *
 */
#version 330 core

layout(std140) uniform Color
{
	vec4 color;
};

out vec4 fscolor;

void main(void)
{
	fscolor = color;
}

)SHADER";

//---------------------------------------------------------------------------
