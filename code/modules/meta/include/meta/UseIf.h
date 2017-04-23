//---------------------------------------------------------------------------

#pragma once

#ifndef META_USE_IF_H
#define META_USE_IF_H

//---------------------------------------------------------------------------

#include "BoolTypes.h"

//---------------------------------------------------------------------------

	/**
	 *  useif, skipif, selectif
	 *
 	 *	@brief
	 *		Used to perform SFINAE operations by the template argument.
	 *		...	- parameters of the operation
	 *
	 *	Examples:
	 *		template<class T, int N_, useif<N != N_>>
	 *		Class & operator = (const T (&val)[N_])
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *
	 *		template<typename T, selectif(0)<same_type<T, String>::value>>
	 *		Class & operator = (const T & val)
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *
	 *		template<typename T, selectif(1)<same_type<T, WideString>::value>>
	 *		Class & operator = (const T & val)
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *	or
	 *		template<int N_, skipif<N == N_>>
	 *		Class<T, N> & operator = (const T (&val)[N_]);
	 *
	 *		template<int N_, skipped_t>
	 *		Class<T, N> & operator = (const T (&val)[N_])
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 */

template<class T, bool ... Test>
struct use_filter_t {};


// S - the sfinae class, t - the enum type, identifies filter, f - the filter, B... - the sequence of filter expressions
#define _useif_class(S)	struct S { enum t { v }; template<bool ... B> using f = typename use_filter_t<S, B...>::type; }
// "S::t = S::t::v" - removes ambiguity by the different S::t enums, "class = S::f" - applies sfinae filter
#define _useif(S)		typename S::t = S::v, class = typename S::f
#define _useif_t(S)		typename S::t, class

_useif_class(sfinae_use);
_useif_class(sfinae_skip);

template<int I>
_useif_class(sfinae_select);

//---------------------------------------------------------------------------

#define useif			_useif(sfinae_use)
#define skipif			_useif(sfinae_skip)
#define selectif(I)		_useif(sfinae_select<I>)

#define used_t			_useif_t(sfinae_use)
#define skipped_t		_useif_t(sfinae_skip)
#define selected_t(I)	_useif_t(sfinae_select<I>)

//---------------------------------------------------------------------------

template<>
struct use_filter_t<sfinae_use> : asd::identity<typename sfinae_use::t> {};

template<bool ... Others>
struct use_filter_t<sfinae_use, true, Others...> : use_filter_t<sfinae_use, Others...> {};

template<bool ... Others>
struct use_filter_t<sfinae_use, false, Others...> {};

template<int I>
struct use_filter_t<sfinae_select<I>> : asd::identity<typename sfinae_select<I>::t> {};

template<int I, bool ... Others>
struct use_filter_t<sfinae_select<I>, true, Others...> : use_filter_t<sfinae_select<I>, Others...> {};

template<int I, bool ... Others>
struct use_filter_t<sfinae_select<I>, false, Others...> {};

template<bool ... Others>
struct use_filter_t<sfinae_skip, false, Others...> : asd::identity<typename sfinae_skip::t> {};

template<bool ... Others>
struct use_filter_t<sfinae_skip, true, Others...> : use_filter_t<sfinae_skip, Others...> {};

template<>
struct use_filter_t<sfinae_skip> {};

//---------------------------------------------------------------------------
#endif
