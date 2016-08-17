//---------------------------------------------------------------------------

#pragma once

#ifndef META_TYPES_H
#define META_TYPES_H

//---------------------------------------------------------------------------

#include "Macro.h"

#include <tuple>
#include <array>
#include <stdint.h>

#ifndef _MSC_VER
#include <type_traits>
#include <initializer_list>
#endif

#ifndef _MSC_VER
namespace std
{
    template<class T>
    using remove_extent_t = typename remove_extent<T>::type;

    template<class T>
    using make_signed_t = typename make_signed<T>::type;

    template<class T>
    using make_unsigned_t = typename make_unsigned<T>::type;

    template<bool value>
    using bool_constant = integral_constant<bool, value>;
}
#endif // _MSC_VER

namespace Rapture
{
	using std::move;
	using std::forward;
	using std::swap;
	using std::declval;
	using std::make_tuple;

	using std::is_const;
	using std::is_array;
	using std::is_pointer;
	using std::is_lvalue_reference;
	using std::is_rvalue_reference;

	using std::is_empty;
	using std::is_abstract;
	using std::is_function;
	using std::is_class;

	using std::is_same;
	using std::is_base_of;
	using std::is_convertible;

	using std::true_type;
	using std::false_type;

	template<bool value>
	using bool_type = std::bool_constant<value>;

	using std::decay_t;
	using std::remove_extent_t;
	using std::remove_cv_t;
	using std::remove_reference_t;

	using std::enable_if_t;
	using std::conditional_t;

	using std::result_of_t;

	using std::tuple;
	using std::array;
	using std::nullptr_t;

	using std::make_integer_sequence;
	using std::make_index_sequence;
	using std::integer_sequence;
	using std::index_sequence;
	using std::index_sequence_for;

	template<class T>
	using remove_ptr_t		= typename std::remove_pointer<T>::type;
	template<class T>
	using clear_t			= remove_ptr_t<decay_t<T>>;
	template<class T>
	using remove_cv_ref_t	= typename std::remove_cv<typename std::remove_reference<T>::type>::type;

//---------------------------------------------------------------------------

	template<class T>
	struct identity
	{
		using type = T;
	};

//---------------------------------------------------------------------------

	/**
	 *  @brief
	 *  Can be used as clear function argument (Empty instead of void *)
	 */
	class Empty {};

	static Empty nothing;

//---------------------------------------------------------------------------

	template<typename T>
	struct Type {};

	template<typename ... T>
	struct Types {};

#define gettype(...) Type<__VA_ARGS__>()
#define gettypes(...) Types<__VA_ARGS__>()

//---------------------------------------------------------------------------

	typedef long long 			llong;

	typedef unsigned char 		byte;
	typedef unsigned short  	ushort;
	typedef unsigned int  		uint;
	typedef unsigned long 		ulong;
	typedef unsigned long long  ullong;

#ifdef __GNUC__
	typedef int              	errno_t;
	typedef long              __time32_t;
	typedef __int64           __time64_t;
#endif // __GNUC__

	typedef unsigned __int8   __uint8;
	typedef unsigned __int16  __uint16;
	typedef unsigned __int32  __uint32;
	typedef unsigned __int64  __uint64;

	typedef __int64 int64;

#ifndef _SSIZE_T_
#define _SSIZE_T_
	using ssize_t = std::make_signed_t<size_t>;
#endif // ssize_t

	template<class T, size_t N>
	using array_t = T[N];

//---------------------------------------------------------------------------

	typedef array<byte, 2>	byte2;
	typedef array<byte, 3>	byte3;
	typedef array<byte, 4>	byte4;

	typedef array<int, 2>	int2;
	typedef array<int, 3>	int3;
	typedef array<int, 4>	int4;

	typedef array<float, 2>	float2;
	typedef array<float, 3>	float3;
	typedef array<float, 4>	float4;

	typedef initializer_list<byte>	byte_list;
	typedef initializer_list<int>	int_list;
	typedef initializer_list<float> float_list;

//---------------------------------------------------------------------------

	template<typename T>
	struct array_size
	{
		static const size_t value = 0;
	};

	template<typename T, size_t N>
	struct array_size<T[N]>
	{
		static const size_t value = N;
	};

	template<typename T, size_t N>
	struct array_size<array<T, N>>
	{
		static const size_t value = N;
	};
}

//---------------------------------------------------------------------------
#endif
