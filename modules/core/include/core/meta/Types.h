//---------------------------------------------------------------------------

#ifndef META_TYPES_H
#define META_TYPES_H

//---------------------------------------------------------------------------

#include <utility>
#include <tuple>
#include "Macro.h"
#include <stdint.h>

#ifndef _MSC_VER
#include <type_traits>
#include <initializer_list>
#endif

namespace Rapture
{
#ifndef _MSC_VER
	namespace std
	{
		template<class T>
		using decay_t = typename decay<T>::type;

		template<class T>
		using remove_extent_t = typename remove_extent<T>::type;

		template<bool cond, class T>
		using enable_if_t = typename enable_if<cond, T>::type;
	}
#endif // _MSC_VER

	using std::move;
	using std::forward;
	using std::swap;
	using std::declval;
	using std::make_tuple;

	using std::is_const;
	using std::is_array;
	using std::is_lvalue_reference;
	using std::is_rvalue_reference;
	using std::is_abstract;

	using std::is_same;
	using std::is_base_of;
	using std::true_type;
	using std::false_type;

	template<bool value>
	using bool_type = std::bool_constant<value>;

	using std::decay_t;
	using std::remove_extent_t;

	using std::enable_if_t;
	using std::conditional_t;

	using std::result_of_t;

	using std::_Wrap_int;
	using std::_Identity;

	using std::initializer_list;
	using std::tuple;

	template<typename T, size_t N>
	using array_t = T[N];

	template<typename ... T>
	using Tuple = tuple<T...>;

	template<class T>
	using remove_ptr_t = typename std::remove_pointer<T>::type;

	template<class T>
	using clear_t = remove_ptr_t<decay_t<T>>;

	template<class T, class Example>
	using adapt_t = conditional_t<is_rvalue_reference<Example>::value, T &&, const T &>;

#define declare_flag_class(name, checker)	\
	class name {};							\
											\
	template<class T>						\
	using checker = is_base_of<name, T>;	\

#define template_constant(name)	\
	template<class T>			\
	struct name					\
	{							\
		static const T value;	\
	};							\

	inline size_t addr(const void * ptr)
	{
		return reinterpret_cast<size_t>(ptr);
	}

	template<int offset>
	struct bitmask
	{
		static const size_t value = 1 << offset;

		template<typename I>
		static bool state(I s)
		{
			return check_flag(value, s);
		}

		template<typename I>
		static void setState(I & s)
		{
			set_flag(value, s);
		}

		template<typename I>
		static void clearState(I & s)
		{
			clear_flag(value, s);
		}
	};

	struct bits
	{
		template<typename I>
		static bool state(int offset, I s)
		{
			return check_flag(I(1 << offset), s);
		}

		template<typename I>
		static void setState(int offset, I & s)
		{
			set_flag(I(1 << offset), s);
		}

		template<typename I>
		static void clearState(int offset, I & s)
		{
			clear_flag(I(1 << offset), s);
		}
	};

	using lo_bit_mask = bitmask<0>;
	using hi_bit_mask = bitmask<15>;

	template<size_t V, int offset>
	struct has_bit
	{
		static const bool value = (V >> offset) & 1;
	};

	/**
	*  @brief
	*  Can be used as clear function argument (const Empty & instead of void *).
	*  Also it is needed for template-class identification (e.g. Template<Empty>::meta)
	*/
	class Empty {};

	static Empty emptiness;

	using std::pair;
	using std::tuple;
	using std::make_integer_sequence;
	using std::make_index_sequence;
	using std::integer_sequence;
	using std::index_sequence;

	template<class ... A>
	inline tuple<A...> wrap(A &&... args)
	{
		return tuple<A...>(forward<A>(args)...);
	}

	typedef unsigned char byte;
	typedef unsigned int  uint;
	typedef unsigned long ulong;

	typedef unsigned __int8  __uint8;
	typedef unsigned __int16 __uint16;
	typedef unsigned __int32 __uint32;
	typedef unsigned __int64 __uint64;

	typedef __int64 int64;

#ifndef _SSIZE_T_
#define _SSIZE_T_
	typedef long long ssize_t;
#endif // ssize_t

	typedef int		int2[2];
	typedef int		int3[3];
	typedef int		int4[4];

	typedef float	float2[2];
	typedef float	float3[3];
	typedef float	float4[4];

	typedef initializer_list<int>	int_list;
	typedef initializer_list<float> float_list;
}

//---------------------------------------------------------------------------
#endif
