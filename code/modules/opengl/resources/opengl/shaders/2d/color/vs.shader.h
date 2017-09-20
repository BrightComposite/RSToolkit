//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char * const shader_code_2d_color_vs = R"SHADER(
/**
 *	!vertex: p2
 */
#version 330 core

in vec2 position;

void main(void) {
	gl_Position = vec4(position, 0.0, 1.0);
}

)SHADER";

//---------------------------------------------------------------------------

static const ::asd::opengl::vertex_layout & shader_code_2d_color_layout = ::asd::opengl::vertex_layouts::p2::get();

//---------------------------------------------------------------------------
