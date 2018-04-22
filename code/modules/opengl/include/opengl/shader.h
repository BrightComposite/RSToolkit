//---------------------------------------------------------------------------

#pragma once

#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <opengl/shaders/code.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		struct shader_program
		{
			deny_copy(shader_program);

			api(opengl)
			shader_program(context & ctx, const shader_code::store & code);

			api(opengl)
			shader_program(shader_program && program) throw();

			api(opengl)
			virtual ~shader_program();

			api(opengl)
			void apply() const;

			const vertex_layout & layout;

		protected:
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
