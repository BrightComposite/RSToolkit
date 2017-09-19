//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

static constexpr char const shader_code_3d_multicolor_vs[] = R"SHADER(
/**
 *	!vertex: p3 c4
 */
#version 330 core

layout(std140) uniform Model
{
	mat4 model;
};

layout(std140) uniform View
{
	mat4 view;
};

layout(std140) uniform Projection
{
	mat4 projection;
};

in vec3 position;
in vec4 color;

out Vertex 
{
	vec4 color;
} output;

void main(void)
{
	output.color = color;
    gl_Position = projection * view * model * vec4(position, 1.0);
}

)SHADER";

//---------------------------------------------------------------------------

static const ::asd::opengl::vertex_layout & shader_code_3d_multicolor_layout = ::asd::opengl::vertex_layouts::p3c4::get();


//---------------------------------------------------------------------------
