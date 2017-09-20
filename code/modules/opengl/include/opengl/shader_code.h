//---------------------------------------------------------------------------

#pragma once

#ifndef SHADER_CODE_H
#define SHADER_CODE_H

//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		namespace shader_code
		{
			struct unit
			{
				const char * const code;
				uint type;
			};

			struct store
			{
				template<uint N>
				store(const vertex_layout & layout, const unit (&units)[N]) :
					units(units), layout(layout) {}

				data<const unit> units;
				const vertex_layout & layout;
			};

			api(opengl) store & get(const std::string & name);
		}
	}
}

//---------------------------------------------------------------------------
#endif
