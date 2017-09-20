//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char * const shader_code_2d_text_fs = R"SHADER(
/**
 *
 */
#version 330 core

layout(std140) uniform Color
{
	vec4 color;
};

uniform sampler2D texture0;

in Vertex 
{
	vec2 texcoord;
} vtx;

out vec4 fscolor;

void main(void)
{
	fscolor = vec4(color[0], color[1], color[2], texture2D(texture0, vtx.texcoord)[0] * color[3]);
}

)SHADER";

//---------------------------------------------------------------------------
