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
#define _useif_class(S)	struct S { enum class t { v }; template<bool ... B> using f = typename use_filter_t<S, B...>::type; }
// "S::t = S::t::v" - removes ambiguity by the different S::t enums, "class = S::f" - applies sfinae filter
#define _useif(S)		S::t = S::t::v, class = S::f
#define _useif_t(S)		S::t, class

_useif_class(SfinaeUse);
_useif_class(SfinaeSkip);

template<int I>
_useif_class(SfinaeSelect);

//---------------------------------------------------------------------------

#define useif			_useif(SfinaeUse)
#define skipif			_useif(SfinaeSkip)
#define selectif(I)		_useif(SfinaeSelect<I>)

#define used_t			_useif_t(SfinaeUse)
#define skipped_t		_useif_t(SfinaeSkip)
#define selected_t(I)	_useif_t(SfinaeSelect<I>)

//---------------------------------------------------------------------------

template<>
struct use_filter_t<SfinaeUse> : Rapture::identity<typename SfinaeUse::t> {};

template<bool ... Others>
struct use_filter_t<SfinaeUse, true, Others...> : use_filter_t<SfinaeUse, Others...> {};

template<bool ... Others>
struct use_filter_t<SfinaeUse, false, Others...> {};

template<int I>
struct use_filter_t<SfinaeSelect<I>> : Rapture::identity<typename SfinaeSelect<I>::t> {};

template<int I, bool ... Others>
struct use_filter_t<SfinaeSelect<I>, true, Others...> : use_filter_t<SfinaeSelect<I>, Others...> {};

template<int I, bool ... Others>
struct use_filter_t<SfinaeSelect<I>, false, Others...> {};

template<bool ... Others>
struct use_filter_t<SfinaeSkip, false, Others...> : Rapture::identity<typename SfinaeSkip::t> {};

template<bool ... Others>
struct use_filter_t<SfinaeSkip, true, Others...> : use_filter_t<SfinaeSkip, Others...> {};

template<>
struct use_filter_t<SfinaeSkip> {};

//---------------------------------------------------------------------------
#endif
