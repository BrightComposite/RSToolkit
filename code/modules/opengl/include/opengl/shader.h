//---------------------------------------------------------------------------

#pragma once

#ifndef SHADER_H
#define SHADER_H

//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <opengl/shaders/code.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		struct shader_program : public opengl::modifier
		{
			deny_copy(shader_program);

			api(opengl)
			shader_program(context & ctx, const shader_code::store & code);

			api(opengl)
			shader_program(shader_program && program) throw();

			api(opengl)
			virtual ~shader_program();

			const vertex_layout & layout() const {
				return _layout;
			}

			api(opengl)
			void apply(context & ctx) const override;

			const vertex_layout & _layout;
			uint id = 0;
		};
		
		namespace shader_code
		{
			api(opengl)
			store & get(const std::string & name);
		}
	}
}

//---------------------------------------------------------------------------
#endif
