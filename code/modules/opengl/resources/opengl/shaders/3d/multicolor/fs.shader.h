//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char * const shader_code_3d_multicolor_fs = R"SHADER(
/**
 *
 */
#version 330 core

in Vertex 
{
	vec4 color;
} vtx;

out vec4 fscolor;

void main(void)
{
	fscolor = vtx.color;
}

)SHADER";

//---------------------------------------------------------------------------
