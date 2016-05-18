//---------------------------------------------------------------------------

#ifndef META_MACRO_H
#define META_MACRO_H

//---------------------------------------------------------------------------

#include <cstddef>
#include <utility>
#include "Preprocessor.h"
#include "Flag.h"

#if defined(__x86_64) || defined(_WIN64)
#define ARCH_X64
#else
#define ARCH_X86
#endif // defined

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
#define __vectorcall
#endif

namespace Rapture
{
    using std::forward;
    using std::move;
	using std::pair;
	using std::initializer_list;

	template<class T, class Y>
	constexpr int compare(const T & x, const Y & y)
	{
		return x > y ? 1 : x < y ? -1 : 0;
	}

	template<class T, class Y>
	constexpr int icomp(const T & x, const Y & y)
	{
		return x >= y ? 1 : -1;
	}

	template<class T>
	size_t max_index(initializer_list<T> list)
	{
		return std::max_element(list.begin(), list.end()) - list.begin();
	}

	template<class T>
	size_t min_index(initializer_list<T> list)
	{
		return std::min_element(list.begin(), list.end()) - list.begin();
	}

	template<class T>
	pair<size_t, size_t> minmax_index(initializer_list<T> list)
	{
		auto minmax = std::minmax_element(list.begin(), list.end());
		return {minmax.first - list.begin(), minmax.second - list.begin()};
	}

	template<class T>
	constexpr bool between(T value, T min, T max)
	{
		return value >= min && value <= max;
	}

	template<class T, class Y>
	constexpr auto align(T x, Y a)
	{
		return ((x - 1) | (a - 1)) + 1;
	}

	template<class T>
	void * void_ptr(T * ptr)
	{
		return reinterpret_cast<void *>(ptr);
	}

	template<class T>
	const void * void_ptr(const T * ptr)
	{
		return reinterpret_cast<const void *>(ptr);
	}

	template<class T>
	void * void_ptr(T & val)
	{
		return reinterpret_cast<void *>(&val);
	}

	template<class T>
	const void * void_ptr(const T & val)
	{
		return reinterpret_cast<const void *>(&val);
	}

	template<class T>
	constexpr size_t addr(const T & val)
	{
		return reinterpret_cast<size_t>(void_ptr(val));
	}

	template<class T>
	constexpr size_t addr(const T * ptr)
	{
		return reinterpret_cast<size_t>(void_ptr(ptr));
	}

	template<int offset, typename I>
	constexpr auto get_bit(I value)
	{
		return (value >> offset) & 0x1;
	}

	template<typename I>
	constexpr auto get_bit(I value, const int offset)
	{
		return (value >> offset) & 0x1;
	}

	template<typename T, typename ... A>
	T * construct(void * place, A &&... args)
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
