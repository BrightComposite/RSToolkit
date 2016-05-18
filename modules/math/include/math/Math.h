//---------------------------------------------------------------------------

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
		static const T one;
		static const T pi;
		static const T pi2;
		static const T half_pi;
		static const T half_pi3;
		static const T inv_pi;
		static const T inv_pi2;
		static const T degree180;
		static const T degree360;
		static const T eps;
		static const T eps2;
		static const T eps3;
		static const T infinity;

		static const T degreetorad;
		static const T radtodegree;
	};

	template<class T>
	struct MathCoefficients
	{
		static const T sin[];
		static const T cos[];
		static const T tan[];
		static const T arc[];
		static const T atan[];
	};

	template<class T>
	using Cfs = MathCoefficients<T>;

	using FloatCfs = Cfs<float>;
	using DoubleCfs = Cfs<double>;

	template<class T, bool integral = std::is_integral<T>::value>
	struct BasicMath
	{
		static inline T abs(const T & x)
		{
			return std::abs(x);
		}

		static inline T sqr(const T & x)
		{
			return x * x;
		}

		static inline T avg(const T & x, const T & y)
		{
			return 0.5f * (x + y);
		}

		static inline void clamp(T & x, const T & low, const T & high)
		{
			if(x < low)
				x = low;
			else
			if(x > high)
				x = high;
		}

		static inline T invert(const T & v)
		{
			return 1 / v;
		}

		static inline T round(const T & v)
		{
			return std::round(v);
		}
	};

	template<class T>
	struct BasicMath<T, true>
	{
		static inline T abs(const T & x)
		{
			return std::abs(x);
		}

		static inline T sqr(const T & x)
		{
			return x * x;
		}

		static inline T avg(const T & x, const T & y)
		{
			return (x + y) >> 1;
		}

		static inline void clamp(T & x, const T & low, const T & high)
		{
			if(x < low)
				x = low;
			else
			if(x > high)
				x = high;
		}

		static inline T invert(const T & v)
		{
			return 1 / v;
		}

		static inline T round(const T & v)
		{
			return v;
		}
	};

	template<class T>
	inline T avg(const T & x, const T & y)
	{
		return BasicMath<T>::avg(x, y);
	}

	template<class T>
	inline T round(const T & x)
	{
		return BasicMath<T>::round(x);
	}

	template<class T>
	inline T sqr(const T & x)
	{
		return BasicMath<T>::sqr(x);
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

	using FloatMath = Math<float>;
	using DoubleMath = Math<double>;

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
}

//---------------------------------------------------------------------------
#endif
