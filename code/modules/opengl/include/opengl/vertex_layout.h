//---------------------------------------------------------------------------

#pragma once

#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

//---------------------------------------------------------------------------

#include <container/array_list.h>
#include <algorithm>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		namespace vertex_layouts
		{
			template<class ... E>
			struct generator;
		}

		struct vertex_layout_element
		{
			const char * name;
			uint units;
		};

		struct vertex_layout
		{
			uint units;
			array_list<vertex_layout_element> elements;

		private:
			vertex_layout(uint units, array_list<vertex_layout_element> && elements) : units(units), elements(std::forward<array_list<vertex_layout_element>>(elements)) {}

			template<class...>
			friend struct vertex_layouts::generator;
		};

		namespace vertex_layouts
		{
			template<class ... E>
			struct generator
			{
				static const uint units = sum_t<uint, E::units...>::value;

				static constexpr const char * key() {
					return concat_t<typename E::key...>::value;
				}

				api(opengl) static const vertex_layout & get() {
					static const vertex_layout l {units, {{concat_t<typename E::name>::value, E::units}...}};
					return l;
				}
			};

			struct position2
			{
				using key = char_sequence<'p','2'>;
				using name = char_sequence<'p','o','s','i','t','i','o','n'>;
				static const uint units = 2;
			};

			struct position3
			{
				using key = char_sequence<'p', '3'>;
				using name = char_sequence<'p','o','s','i','t','i','o','n'>;
				static const uint units = 3;
			};

			struct color3
			{
				using key = char_sequence<'c', '3'>;
				using name = char_sequence<'c','o','l','o','r'>;
				static const uint units = 3;
			};

			struct color4
			{
				using key = char_sequence<'c', '4'>;
				using name = char_sequence<'c','o','l','o','r'>;
				static const uint units = 3;
			};

			struct texture
			{
				using key = char_sequence<'t'>;
				using name = char_sequence<'t','e','x','t','u','r','e'>;
				static const uint units = 2;
			};

			using p2		= generator<position2>;
			using p3		= generator<position2>;
			using p2c3		= generator<position2, color3>;
			using p3c3		= generator<position3, color3>;
			using p2c4		= generator<position2, color4>;
			using p3c4		= generator<position3, color4>;
			using p2t		= generator<position2, texture>;
			using p3t		= generator<position3, texture>;
			using p2c3t		= generator<position2, color3, texture>;
			using p3c3t		= generator<position3, color3, texture>;
			using p2c4t		= generator<position2, color4, texture>;
			using p3c4t		= generator<position3, color4, texture>;

			struct storage
			{
				api(opengl) storage();

				api(opengl) const vertex_layout & get(const std::string & key);
			};

			inline const vertex_layout & get(const std::string & key) {
				static storage s;
				return s.get(key);
			}
		}
	}
}

//---------------------------------------------------------------------------
#endif
