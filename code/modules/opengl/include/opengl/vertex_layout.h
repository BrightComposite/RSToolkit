//---------------------------------------------------------------------------

#pragma once

#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

//---------------------------------------------------------------------------

#include <container/array_list.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		struct vertex_layout_element
		{
			uint units;
		};

		struct vertex_layout
		{
			uint units;
			array_list<vertex_layout_element> elements;
		};

		namespace vertex_layouts
		{
			template<class T, T ... E>
			struct sum {};

			template<class T, T Head, T ... Tail>
			struct sum<T, Head, Tail...>
			{
				static constexpr T value = Head + sum<T, Tail...>::value;
			};

			template<class T>
			struct sum<T>
			{
				static constexpr T value = 0;
			};

			template<char ... Ch>
			struct char_sequence
			{
				static constexpr const char value[sizeof...(Ch) + 1] = {Ch..., '\0'};
			};

			template<char ... Ch>
			api(opengl) constexpr const char char_sequence<Ch...>::value[sizeof...(Ch) + 1];

			template<class T, class...> 
			struct concat 
			{
				using type = T;
			};

			template<char... Dest, char ... Src, class... Ts>
			struct concat<char_sequence<Dest...>, char_sequence<Src...>, Ts...> : concat<char_sequence<Dest..., Src...>, Ts...> {};

			template<class ... E>
			struct generator
			{
				using key_type = typename concat<typename E::key...>::type;

				static const uint units = sum<uint, E::units...>::value;

				static constexpr const char * key() {
					return generator::key_type::value;
				}

				static const vertex_layout & get() {
					static const vertex_layout l {units, {{E::units}...}};
					return l;
				}
			};

			struct position2
			{
				using key = char_sequence<'p', '2'>;
				static const uint units = 2;
			};

			struct position3
			{
				using key = char_sequence<'p', '3'>;
				static const uint units = 3;
			};

			struct color3
			{
				using key = char_sequence<'c', '3'>;
				static const uint units = 3;
			};

			struct color4
			{
				using key = char_sequence<'c', '4'>;
				static const uint units = 3;
			};

			struct texture
			{
				using key = char_sequence<'t'>;
				static const uint units = 2;
			};

			using p2 = generator<position2>;
			using p3 = generator<position2>;
			using p2c3 = generator<position2, color3>;
			using p3c3 = generator<position3, color3>;
			using p2c4 = generator<position2, color4>;
			using p3c4 = generator<position3, color4>;
			using p2t = generator<position2, texture>;
			using p3t = generator<position3, texture>;

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
