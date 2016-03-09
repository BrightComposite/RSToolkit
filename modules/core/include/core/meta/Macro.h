//---------------------------------------------------------------------------

#ifndef DEFINE_MACRO_H
#define DEFINE_MACRO_H

//---------------------------------------------------------------------------

#include <stddef.h>
#include "Preprocessor.h"
#include "Flag.h"

#ifndef __min
    #define __min(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef __max
    #define __max(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifdef _MSC_VER
#define forceinline __forceinline
#else
#define forceinline inline
#endif

namespace Rapture
{
	template<class T, class Y>
	inline int compare(const T & x, const Y & y)
	{
		return x > y ? 1 : x < y ? -1 : 0;
	}

	template<class T, class Y>
	inline int icomp(const T & x, const Y & y)
	{
		return x >= y ? 1 : -1;
	}

	template<class T>
	inline bool between(T value, T min, T max)
	{
		return value >= min && value <= max;
	}

	template<class T, class Y>
	inline T align(T x, Y a)
	{
		return ((x - 1) | (a - 1)) + 1;
	}

	template<class T>
	inline void * void_ptr(T * ptr)
	{
		return reinterpret_cast<void *>(ptr);
	}

	template<class T>
	inline const void * void_ptr(const T * ptr)
	{
		return reinterpret_cast<const void *>(ptr);
	}

	template<class T>
	inline void * void_ptr(T & ptr)
	{
		return reinterpret_cast<void *>(&ptr);
	}

	template<class T>
	inline const void * void_ptr(const T & ptr)
	{
		return reinterpret_cast<const void *>(&ptr);
	}

	template<int offset, typename I>
	inline int get_bit(I value)
	{
		return static_cast<int>((value >> offset) & 0x1);
	}

	template<typename I>
	inline int get_bit(I value, const int offset)
	{
		return static_cast<int>((value >> offset) & 0x1);
	}

	template<typename T, typename ... A>
	inline T * construct(void * place, A &&... args)
	{
		return new (place) T(forward<A>(args)...);
	}
}

/**
 *	Use it to implement declarations of class-members in the faster way
 */
#define implement(x, ...) decltype(x) x##__VA_ARGS__;

 /**
  *	Use it to wrap template parameters and prevent incorrect preprocessing
  */
#define macrowrap(...) __VA_ARGS__

/**
 *	Use it to prevent warnings about unused return value
 */
#define absorb(...) auto && ____##__VA_ARGS__ =

/**
 *	Short for-loop
 */
#define repeat(var, times) for(int var = 0; var < times; ++var)

//---------------------------------------------------------------------------
#endif
