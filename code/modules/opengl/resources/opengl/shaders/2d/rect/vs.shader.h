//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char * const shader_code_2d_rect_vs = R"SHADER(
/**
 *	!vertex: p2
 */
#version 330 core

layout(std140) uniform Area
{
	vec2  pos;
	vec2  size;
	float depth;
};

in vec2 position;

void main(void)
{
    gl_Position = vec4(position * size + pos, depth, 1.0);
}

)SHADER";

//---------------------------------------------------------------------------

static const ::asd::opengl::vertex_layout & shader_code_2d_rect_layout = ::asd::opengl::vertex_layouts::p2::get();

//---------------------------------------------------------------------------
