//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char shader_code_2d_image_fs = R"SHADER(
/**
 *
 */
#version 330 core

uniform sampler2D tex;

in Vertex 
{
	vec2 texcoord;
} vtx;

out vec4 fscolor;

void main(void)
{
	fscolor = texture2D(tex, vtx.texcoord);
}

)SHADER";

//---------------------------------------------------------------------------
