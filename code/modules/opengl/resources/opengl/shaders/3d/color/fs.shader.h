//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char * const shader_code_3d_color_fs = R"SHADER(
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
