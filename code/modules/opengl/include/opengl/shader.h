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
		class shader_program
		{
			deny_copy(shader_program);

		public:
			api(opengl)
			shader_program(context & ctx, const shader_code::store & code);

			api(opengl)
			~shader_program();

			api(opengl)
			friend void apply(context & ctx, const shader_program & sh);

		private:
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
