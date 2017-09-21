//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char * const shader_code_2d_color_vs = R"SHADER(
/**
 *	!vertex: p2 c3
 */
#version 330 core

in vec2 position;
in vec3 color;

out Vertex 
{
	vec4 color;
} vtx;

void main(void)
{
	vtx.color = vec4(color, 1.0);
	gl_Position = vec4(position, 0.0, 1.0);
}

)SHADER";

//---------------------------------------------------------------------------

static const ::asd::opengl::vertex_layout & shader_code_2d_color_layout = ::asd::opengl::vertex_layouts::p2c3::get();

//---------------------------------------------------------------------------
