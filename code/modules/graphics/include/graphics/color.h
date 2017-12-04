//---------------------------------------------------------------------------

#pragma once

#ifndef COLOR_H
#define COLOR_H

//---------------------------------------------------------------------------

//#include <_____>

//---------------------------------------------------------------------------

namespace asd
{
	namespace gfx
	{
		template <class T>
		struct rgba
		{
			T r, g, b, a;
		};
		
		template <class T>
		struct argb
		{
			T a, r, g, b;
		};
		
		template <class T>
		struct hsva
		{
			T h, s, v, a;
		};
		
		template <class T>
		struct hsla
		{
			T h, s, l, a;
		};
		
		template <class T>
		struct color_limits {};
		
		template <>
		struct color_limits<int>
		{
			static const int min() {
				return 0;
			}
			
			static const int max() {
				return 255;
			}
		};
		
		template <>
		struct color_limits<float>
		{
			static const float min() {
				return 0.0f;
			}
			
			static const float max() {
				return 1.0f;
			}
		};
		
		template <class T, template <class> class Model = rgba>
		struct color : Model<T>
		{
			using limits = color_limits<T>;
			using model = Model<T>;
			
			color(T x = limits::min(), T y = limits::min(), T z = limits::min(), T alpha = limits::max()) : model {x, y, z, alpha} {}
		};
		
		using colorf = color<float>;
	}
}

//---------------------------------------------------------------------------
#endif
