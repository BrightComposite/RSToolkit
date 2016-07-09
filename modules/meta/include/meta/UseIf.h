//---------------------------------------------------------------------------

#pragma once

#ifndef META_USE_IF_H
#define META_USE_IF_H

//---------------------------------------------------------------------------

#include "BoolTypes.h"

namespace Rapture
{
	template<class SfinaeClass, bool ... Test>
	struct use_filter_t {};
}

//#ifdef MSVC
#define declare_useif_class(Class)														        \
struct Class																			        \
{																						        \
	enum class type { value };																    \
																								\
	template<bool ... Test>																		\
	struct filter : Rapture::use_filter_t<Class, Test...> {};									\
																								\
}
/*#else
#define declare_useif_class(Class)														        \
struct Class																			        \
{																						        \
	enum class type { value };																    \
																								\
	template<bool ... Test, typename = typename Rapture::use_filter_t<Class, Test...>::type>    \
	static constexpr type filter()														        \
	{																					        \
		return type::value;																	    \
	}																					        \
}
#endif // MSVC
*/
declare_useif_class(SfinaeUse);
declare_useif_class(SfinaeSkip);

template<int I>
declare_useif_class(SfinaeSelect);

using useif_t    = typename SfinaeUse::type;

using skipif_t   = typename SfinaeSkip::type;
template<int I>
using selectif_t = typename SfinaeSelect<I>::type;

//---------------------------------------------------------------------------

	/**
	 *	@brief
	 *		Used to perform SFINAE operations by the template argument.
	 *		...	- parameters of the operation
	 *
	 *	Examples:
	 *		template<class T, int N_, useif <N != N_> endif>
	 *		Class & operator = (const T (&val)[N_])
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *
	 *		template<typename T, selectif(0) <same_type<T, String>::value> endif>
	 *		Class & operator = (const T & val)
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *
	 *		template<typename T, selectif(1) <same_type<T, WideString>::value> endif>
	 *		Class & operator = (const T & val)
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *	or
	 *		template<int N_, skipif <N == N_> endif>
	 *		Class<T, N> & operator = (const T (&val)[N_]);
	 *
	 *		template<int N_, skipif_t>
	 *		Class<T, N> & operator = (const T (&val)[N_])
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 */
#define useif		useif_t		  = SfinaeUse::filter
#define skipif		skipif_t	  = SfinaeSkip::filter
#define selectif(I) selectif_t<I> = SfinaeSelect<I>::filter

//#ifdef MSVC
#define endif ::value
/*#else
#define endif ()
#endif
*/
//---------------------------------------------------------------------------

namespace Rapture
{
//#ifdef MSVC
	template<class T, T Val>
	struct use_filter_base : std::integral_constant<T, Val> {};
/*#else
	template<class T, T Val>
	struct use_filter_base : identity<T> {};
#endif // MSVC
*/
	template<>
	struct use_filter_t<SfinaeUse> : use_filter_base<useif_t, useif_t::value> {};

	template<bool ... Others>
	struct use_filter_t<SfinaeUse, true, Others...> : use_filter_t<SfinaeUse, Others...> {};

	template<bool ... Others>
	struct use_filter_t<SfinaeUse, false, Others...> {};

	template<int I>
	struct use_filter_t<SfinaeSelect<I>> : use_filter_base<selectif_t<I>, selectif_t<I>::value> {};

	template<int I, bool ... Others>
	struct use_filter_t<SfinaeSelect<I>, true, Others...> : use_filter_t<SfinaeSelect<I>, Others...> {};

	template<int I, bool ... Others>
	struct use_filter_t<SfinaeSelect<I>, false, Others...> {};

	template<bool ... Others>
	struct use_filter_t<SfinaeSkip, false, Others...> : use_filter_base<skipif_t, skipif_t::value> {};

	template<bool ... Others>
	struct use_filter_t<SfinaeSkip, true, Others...> : use_filter_t<SfinaeSkip, Others...> {};

	template<>
	struct use_filter_t<SfinaeSkip> {};
}

//---------------------------------------------------------------------------
#endif
