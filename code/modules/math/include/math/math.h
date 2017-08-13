//---------------------------------------------------------------------------

#pragma once

#ifndef MATH_DEFS_H
#define MATH_DEFS_H

//---------------------------------------------------------------------------

#define _USE_MATH_DEFINES

#include <meta/Meta.h>

#include <math.h>
#include <algorithm>
#include <float.h>

#include <stdlib.h>

#undef min
#undef max

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template<class T>
		struct constants
		{
			static api(math) const T one;
			static api(math) const T pi;
			static api(math) const T pi2;
			static api(math) const T half_pi;
			static api(math) const T half_pi3;
			static api(math) const T inv_pi;
			static api(math) const T inv_pi2;
			static api(math) const T degree180;
			static api(math) const T degree360;
			static api(math) const T eps;
			static api(math) const T eps2;
			static api(math) const T eps3;
			static api(math) const T infinity;
			
			static api(math) const T degrees2radians;
			static api(math) const T radians2degrees;
		};
		
		template<class T>
		struct coefficients
		{
			static api(math) const T sin[];
			static api(math) const T cos[];
			/*static const T tan[];
			static const T arc[];
			static const T atan[];*/
		};
		
		template<class T>
		using cfs = coefficients<T>;
		
		using float_cfs = cfs<float>;
		using double_cfs = cfs<double>;
		
		using std::is_integral;
		using std::is_floating_point;
		
		template<class T>
		struct is_numeric : bool_type<is_floating_point<T>::value || is_integral<T>::value> {};
		
		template<class T, useif<is_numeric<T>::value>>
		inline T abs(T x) {
			return std::abs(x);
		}
		
		template<class T, selectif(0)<is_floating_point<T>::value>>
		inline T sign(T x) {
			return x > 0.0f ? 1.0f : x < 0.0f ? -1.0f : 0.0f;
		}
		
		template<class T, selectif(1)<is_integral<T>::value>>
		inline T sign(T x) {
			return x > 0 ? 1 : x < 0 ? -1 : 0;
		}
		
		template<class T>
		inline auto sqr(T x) -> decltype(x * x) {
			return x * x;
		}
		
		template<class T, selectif(0)<is_floating_point<T>::value>>
		inline T pow(T x, T pow) {
			return std::pow(x, pow);
		}
		
		template<class T, selectif(1)<is_integral<T>::value>>
		inline T pow(T x, T pow) {
			return static_cast<T>(std::pow(x, pow));
		}
		
		template<class T, selectif(0)<is_floating_point<T>::value>>
		inline T sqrt(T x) {
			return std::sqrt(x);
		}
		
		template<class T, selectif(1)<is_integral<T>::value>>
		inline T sqrt(T x) {
			return static_cast<T>(std::sqrt(x));
		}
		
		template<class T, useif<is_floating_point<T>::value>>
		inline T mod(const T & x, const T & y) {
			return std::fmod(x, y);
		}
		
		template<class T, useif<is_integral<T>::value>>
		inline T mod(T x, T y) {
			return x % y;
		}
		
		template<class T, skipif<is_integral<T>::value>>
		inline auto avg(const T & x, const T & y) -> decltype(0.5f * (x + y)) {
			return 0.5f * (x + y);
		}
		
		template<class T, useif<is_integral<T>::value>>
		inline T avg(T x, T y) {
			return (x + y) >> 1;
		}
		
		template<class T, useif<is_less_comparable<T>::value>>
		inline const T & clamp(const T & x, const T & low, const T & high) {
			return high < x ? high : x < low ? low : x;
		}
		
		template<class T, skipif<is_integral<T>::value>>
		inline T invert(const T & x) {
			return 1 / x;
		}
		
		template<class T, useif<is_integral<T>::value>>
		inline T invert(T x) {
			return x == 0 ? INFINITY : x == 1 ? 1 : 0;
		}
		
		template<class T, selectif(0)<is_floating_point<T>::value>>
		inline T floor(const T & v) {
			return std::floor(v);
		}
		
		template<class T, selectif(1)<is_floating_point<T>::value>>
		inline T floor(T x) {
			return x;
		}
		
		template<class T, selectif(0)<is_floating_point<T>::value>>
		inline T ceil(const T & v) {
			return std::ceil(v);
		}
		
		template<class T, selectif(1)<is_floating_point<T>::value>>
		inline T ceil(T x) {
			return x;
		}
		
		template<class T, selectif(0)<is_floating_point<T>::value>>
		inline T round(const T & v) {
			return std::round(v);
		}
		
		template<class T, selectif(1)<is_floating_point<T>::value>>
		inline T round(T x) {
			return x;
		}
		
		template<class T, class Y>
		inline auto lerp(const T & a, const T & b, const Y & t) -> decltype(a + (b - a) * t) {
			return a + (b - a) * t;
		}
		
		template<class T>
		inline T round2pow(T x) {
			if(x <= 0) {
				return 0;
			}
			
			--x;
			
			for(size_t i = 1; i < sizeof(T) * 8; i <<= 1) {
				x |= x >> i;
			}
			
			return ++x;
		}
		
		template<int I>
		struct factorial
		{
			static const size_t value = I * factorial<I - 1>::value;
			
			template<class T>
			static inline T cast() {
				return T(value);
			}
			
			template<class T>
			static inline T invert() {
				static const T v(math::invert(cast<T>()));
				return v;
			}
		};
		
		template<>
		struct factorial<0>
		{
			static const size_t value = 1;
			
			template<class T>
			static inline T cast() {
				return T(1);
			}
			
			template<class T>
			static inline T invert() {
				return T(1);
			}
		};
		
		template<>
		struct factorial<1>
		{
			static const size_t value = 1;
			
			template<class T>
			static inline T cast() {
				return T(1);
			}
			
			template<class T>
			static inline T invert() {
				return T(1);
			}
		};
		
		template<class T>
		inline T sin(T angle) {
			return std::sin(angle);
		}
		
		template<class T>
		inline T cos(T angle) {
			return std::cos(angle);
		}
		
		template<class T>
		inline void sincos(T angle, T & sine, T & cosine) {
			sine = std::sin(angle);
			cosine = std::cos(angle);
		}
		
		// convert degrees to radians
		template<class T>
		inline T radians(const T & degrees) {
			return constants<T>::degrees2radians * degrees;
		}
		
		// convert radians to degrees
		template<class T>
		inline T degrees(const T & radians) {
			return constants<T>::radians2degrees * radians;
		}
		
		// put an angle into [-pi; pi] range
		template<class T>
		inline T normalize_angle(const T & angle) {
			return angle - round(angle * constants<T>::inv_pi2) * constants<T>::pi2;
		}
		
		enum class relative_pos
		{
			inside = 0x00,
			before = 0x01,
			after = 0x02,
			in_back = static_cast<int>(before),
			in_front = static_cast<int>(after),
			around = 0x03
		};
		
		enum class axis
		{
			x = 0x00,
			y = 0x01,
			z = 0x02
		};
		
		enum class axis_mask
		{
			none = 0x00,
			x = 1 << static_cast<int>(axis::x),
			y = 1 << static_cast<int>(axis::y),
			z = 1 << static_cast<int>(axis::z),
			xy = static_cast<int>(x) | static_cast<int>(y),
			xz = static_cast<int>(x) | static_cast<int>(z),
			yz = static_cast<int>(y) | static_cast<int>(z),
			xyz = static_cast<int>(x) | static_cast<int>(y) | static_cast<int>(z),
		};
		
		template<typename T>
		using limits = std::numeric_limits<T>;
	}
}

//---------------------------------------------------------------------------
#endif
