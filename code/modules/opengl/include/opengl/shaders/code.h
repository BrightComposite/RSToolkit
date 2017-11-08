//---------------------------------------------------------------------------

#pragma once

#ifndef OPENGL_SHADER_CODE_H
#define OPENGL_SHADER_CODE_H

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
				store(const unit (&units)[N], const vertex_layout & layout) :
					units(units), layout(layout) {}

				data<const unit> units;
				const vertex_layout & layout;
			};
		}
	}
}

//---------------------------------------------------------------------------
#endif
