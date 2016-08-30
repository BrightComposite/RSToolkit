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

namespace Rapture
{
	template<class T>
	struct Math;

	template<class T>
	struct MathConstants
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

		static api(math) const T degreetorad;
		static api(math) const T radtodegree;
	};

	template<class T>
	struct MathCoefficients
	{
		static api(math) const T sin[];
		static api(math) const T cos[];
		/*static const T tan[];
		static const T arc[];
		static const T atan[];*/
	};

	template<class T>
	using Cfs = MathCoefficients<T>;

	using FloatCfs = Cfs<float>;
	using DoubleCfs = Cfs<double>;

	enum class MathType
	{
		Integral,
		Floating,
		Other
	};

	template<class T, MathType type = std::is_integral<T>::value ? MathType::Integral : std::is_floating_point<T>::value ? MathType::Floating : MathType::Other>
	struct BasicMath {};

	template<class T>
	struct BasicMath<T, MathType::Floating>
	{
		static inline T abs(const T & x)
		{
			return std::abs(x);
		}

		static inline T sign(const T & x)
		{
			return x > 0.0f ? 1.0f : x < 0.0f ? -1.0f : 0.0f;
		}

		static inline T sqr(const T & x)
		{
			return x * x;
		}

		static inline T pow(const T & x, const T & pow)
		{
			return std::pow(x, pow);
		}

		static inline T sqrt(const T & x)
		{
			return std::sqrt(x);
		}

		static inline T mod(const T & x, const T & y)
		{
			return std::fmod(x, y);
		}

		static inline T avg(const T & x, const T & y)
		{
			return 0.5f * (x + y);
		}

		static inline const T & clamp(const T & x, const T & low, const T & high)
		{
			return x > high ? high : x < low ? low : x;
		}

		static inline T invert(const T & v)
		{
			return 1 / v;
		}

		static inline T floor(const T & v)
		{
			return std::floor(v);
		}

		static inline T ceil(const T & v)
		{
			return std::ceil(v);
		}

		static inline T round(const T & v)
		{
			return std::round(v);
		}

		static inline T lerp(T a, T b, T t)
		{
			return a + (b - a) * t;
		}
	};

	template<class T>
	struct BasicMath<T, MathType::Integral>
	{
		static inline T abs(const T & x)
		{
			return std::abs(x);
		}

		static inline T sign(const T & x)
		{
			return x > 0 ? 1 : x < 0 ? -1 : 0;
		}

		static inline T sqr(const T & x)
		{
			return x * x;
		}

		static inline T pow(const T & x, const T & pow)
		{
			return static_cast<T>(std::pow(x, pow));
		}

		static inline T sqrt(const T & x)
		{
			return static_cast<T>(std::sqrt(x));
		}

		static inline T mod(const T & x, const T & y)
		{
			return x % y;
		}

		static inline T avg(const T & x, const T & y)
		{
			return (x + y) >> 1;
		}

		static inline const T & clamp(const T & x, const T & low, const T & high)
		{
			return x > high ? high : x < low ? low : x;
		}

		static inline T invert(const T & v)
		{
			return 1 / v;
		}

		static inline T floor(const T & v)
		{
			return v;
		}

		static inline T ceil(const T & v)
		{
			return v;
		}

		static inline T round(const T & v)
		{
			return v;
		}

		template<class U, useif<not_same_type<U, double>::value>>
		static inline T lerp(T a, T b, U t)
		{
			return a + (b - a) * static_cast<float>(t);
		}

		static inline T lerp(T a, T b, double t)
		{
			return a + (b - a) * t;
		}

		static inline T round2pow(const T & v)
		{
			T i = 1;

			while(i < v)
				i <<= 1;

			return i;
		}
	};

	template<class T>
	struct BasicMath<T, MathType::Other>
	{
		static inline T abs(const T & x)
		{
			return std::abs(x);
		}

		static inline T sign(const T & x)
		{
			return x > 0 ? 1 : x < 0 ? -1 : 0;
		}

		static inline T sqr(const T & x)
		{
			return x * x;
		}

		static inline T pow(const T & x, const T & pow)
		{
			return static_cast<T>(std::pow(x, pow));
		}

		static inline T sqrt(const T & x)
		{
			return static_cast<T>(std::sqrt(x));
		}

		static inline T avg(const T & x, const T & y)
		{
			return (x + y) >> 1;
		}

		static inline const T & clamp(const T & x, const T & low, const T & high)
		{
			return x > high ? high : x < low ? low : x;
		}

		static inline T invert(const T & v)
		{
			return 1 / v;
		}

		static inline T floor(const T & v)
		{
			return v;
		}

		static inline T ceil(const T & v)
		{
			return v;
		}

		static inline T round(const T & v)
		{
			return v;
		}

		template<class U, useif<not_same_type<U, double>::value>>
		static inline T lerp(T a, T b, U t)
		{
			return a + (b - a) * static_cast<float>(t);
		}

		static inline T lerp(T a, T b, double t)
		{
			return a + (b - a) * t;
		}
	};

	template<class T>
	inline auto avg(const T & x, const T & y) -> decltype(BasicMath<T>::avg(x, y))
	{
		return BasicMath<T>::avg(x, y);
	}

	template<class T>
	inline auto floor(const T & x) -> decltype(BasicMath<T>::floor(x))
	{
		return BasicMath<T>::floor(x);
	}

	template<class T>
	inline auto ceil(const T & x) -> decltype(BasicMath<T>::ceil(x))
	{
		return BasicMath<T>::ceil(x);
	}

	template<class T>
	inline auto round(const T & x) -> decltype(BasicMath<T>::round(x))
	{
		return BasicMath<T>::round(x);
	}

	template<class T>
	inline auto sqr(const T & x) -> decltype(BasicMath<T>::sqr(x))
	{
		return BasicMath<T>::sqr(x);
	}

	template<class T, class U>
	inline auto lerp(const T & a, const T & b, U t) -> decltype(BasicMath<T>::lerp(a, b, t))
	{
		return BasicMath<T>::lerp(a, b, t);
	}

	template<int I>
	struct factorial
	{
		static const size_t value = I * factorial<I - 1>::value;

		template<class T>
		static inline T cast()
		{
			return T(value);
		}

		template<class T>
		static inline T invert()
		{
			static const T v(Math<T>::invert(cast<T>()));
			return v;
		}
	};

	template<>
	struct factorial<0>
	{
		static const size_t value = 1;

		template<class T>
		static inline T cast()
		{
			return T(1);
		}

		template<class T>
		static inline T invert()
		{
			return T(1);
		}
	};

	template<>
	struct factorial<1>
	{
		static const size_t value = 1;

		template<class T>
		static inline T cast()
		{
			return T(1);
		}

		template<class T>
		static inline T invert()
		{
			return T(1);
		}
	};

	template<class T>
	struct SinCos
	{
		static inline T sin(T angle)
		{
			return std::sin(angle);
		}

		static inline T cos(T angle)
		{
			return std::cos(angle);
		}

		static inline void sincos(T angle, T & sine, T & cosine)
		{
			sine = std::sin(angle);
			cosine = std::cos(angle);
		}
	};

	template<class T>
	struct Trigonometry : SinCos<T>
	{
		// convert degrees to radians
		static inline T dtor(const T & x)
		{
			return Math<T>::degreetorad * x;
		}

		// convert radians to degrees
		static inline T rtod(const T & x)
		{
			return Math<T>::radtodegree * x;
		}

		// put an angle into [-pi; pi] range
		static inline T rmod(const T & angle)
		{
			return angle - Math<T>::round(angle * Math<T>::inv_pi2) * Math<T>::pi2;
		}
	};

	template<class T>
	struct Math : MathConstants<T>, BasicMath<T>, Trigonometry<T> {};

	template<class T>
	inline T rad(const T & degrees)
	{
		return Math<T>::dtor(degrees);
	}

	using FloatMath = Math<float>;
	using DoubleMath = Math<double>;
	using IntMath = Math<int>;
	using UintMath = Math<uint>;

	using fmath = FloatMath;
	using dmath = DoubleMath;
	using imath = IntMath;
	using umath = UintMath;

	enum class RelativePosition
	{
		Inside  = 0x00,
		Before  = 0x01,
		After   = 0x02,
		InBack  = Before,
		InFront = After,
		Around  = 0x03
	};

	enum class Axis
	{
		X = 0x00,
		Y = 0x01,
		Z = 0x02,

		Horizontal = X,
		Vertical = Y
	};

	enum class AxisMask
	{
		AxisMaskNone = 0x00,
		AxisMaskX    = 0x01,
		AxisMaskY    = 0x02,
		AxisMaskZ    = 0x04,
		AxisMaskXY   = AxisMaskX | AxisMaskY,
		AxisMaskXZ   = AxisMaskX | AxisMaskZ,
		AxisMaskYZ   = AxisMaskY | AxisMaskZ,
		AxisMaskXYZ  = AxisMaskX | AxisMaskY | AxisMaskZ,
	};

	template<typename T>
	using limits = std::numeric_limits<T>;
}

//---------------------------------------------------------------------------
#endif
