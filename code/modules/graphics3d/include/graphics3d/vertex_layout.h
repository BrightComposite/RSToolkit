//---------------------------------------------------------------------------

#pragma once

#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

//---------------------------------------------------------------------------

#include <meta/meta.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace gfx3d
	{
		namespace elements
		{
			struct position2
			{
				using key = char_sequence<'p', '2'>;
				using name = char_sequence<'p', 'o', 's', 'i', 't', 'i', 'o', 'n'>;
				static const uint units = 2;
			};

			struct position3
			{
				using key = char_sequence<'p', '3'>;
				using name = char_sequence<'p', 'o', 's', 'i', 't', 'i', 'o', 'n'>;
				static const uint units = 3;
			};

			struct position4
			{
				using key = char_sequence<'p', '4'>;
				using name = char_sequence<'p', 'o', 's', 'i', 't', 'i', 'o', 'n'>;
				static const uint units = 4;
			};

			struct color3
			{
				using key = char_sequence<'c', '3'>;
				using name = char_sequence<'c', 'o', 'l', 'o', 'r'>;
				static const uint units = 3;
			};

			struct color4
			{
				using key = char_sequence<'c', '4'>;
				using name = char_sequence<'c', 'o', 'l', 'o', 'r'>;
				static const uint units = 3;
			};

			struct texture
			{
				using key = char_sequence<'t'>;
				using name = char_sequence<'t', 'e', 'x', 't', 'u', 'r', 'e'>;
				static const uint units = 2;
			};
		}
	}
}

//---------------------------------------------------------------------------
#endif
