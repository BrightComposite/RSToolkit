//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

static const char * const shader_code_3d_texture_vs = R"SHADER(
/**
 *	!vertex: p3 t
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
in vec2 texcoord;

out Vertex 
{
	vec2 texcoord;
} output;

void main(void)
{
    output.texcoord = texcoord;
    gl_Position = projection * view * model * vec4(position, 1.0);
}

)SHADER";

//---------------------------------------------------------------------------

static const ::asd::opengl::vertex_layout & shader_code_3d_texture_layout = ::asd::opengl::vertex_layouts::p3t::get();

//---------------------------------------------------------------------------
