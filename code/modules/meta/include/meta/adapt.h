//---------------------------------------------------------------------------

#pragma once

#ifndef META_ADAPT_H
#define META_ADAPT_H

//---------------------------------------------------------------------------

#include "useif.h"

//---------------------------------------------------------------------------

namespace asd
{
	template<class T, class Example>
	struct adapt_type
	{
		using type = T;
	};

	template<class T, class Example>
	struct adapt_type<T, const Example>
	{
		using type = const T;
	};

	template<class T, class Example>
	struct adapt_type<T, const Example &>
	{
		using type = const T;
	};

	template<class T, class Example>
	struct adapt_type<T, Example &>
	{
		using type = T &;
	};

	template<class T, class Example>
	struct adapt_type<T, Example &&>
	{
		using type = T &&;
	};

	template<class T, class Example>
	using adapt_t = typename adapt_type<T, Example>::type;

	template<class T, class E, class R = remove_reference_t<E>, class U = adapt_t<T, R>, selectif(0)<is_convertible<R &, U &>::value>>
	constexpr U & adapt(R & arg)
	{
		return static_cast<U &>(arg);
	}

	template<class T, class E, class R = remove_reference_t<E>, class U = adapt_t<T, R>, selectif(0)<is_convertible<R &&, U &&>::value>>
	constexpr U adapt(R && arg)
	{
		return static_cast<U &&>(forward<R>(arg));
	}

	template<class T, class E, class R = remove_reference_t<E>, class U = adapt_t<T, R>, selectif(1)<is_convertible<R &, U>::value, !is_convertible<R &, U &>::value>>
	constexpr U adapt(R & arg)
	{
		return static_cast<U>(arg);
	}

	template<class T, class E, class R = remove_reference_t<E>, class U = adapt_t<T, R>, selectif(1)<is_convertible<R &&, U>::value, !is_convertible<R &&, U &&>::value>>
	constexpr U adapt(R && arg)
	{
		return static_cast<U>(arg);
	}
}

//---------------------------------------------------------------------------
#endif
