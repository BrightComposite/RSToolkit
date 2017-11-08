//---------------------------------------------------------------------------

#pragma once

#ifndef OPENGL_VERTEX_LAYOUT_H
#define OPENGL_VERTEX_LAYOUT_H

//---------------------------------------------------------------------------

#include <algorithm>
#include <container/array_list.h>
#include <graphics3d/vertex_layout.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		namespace vertex_layouts
		{
			using namespace gfx3d::elements;

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

			bool operator == (const vertex_layout & l) const {
				return this == &l;
			}

			bool operator != (const vertex_layout & l) const {
				return this != &l;
			}

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
