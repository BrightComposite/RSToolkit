//---------------------------------------------------------------------------

#ifndef REQUIRE_H
#define REQUIRE_H

//---------------------------------------------------------------------------

#include "Preprocessor.h"

//---------------------------------------------------------------------------

	/**
	 *	@brief
	 *		Used to perform SFINAE operations by the return type.
	 *		ret	- return type of the function
	 *		...	- parameters of the operation
	 *
	 *	Example:
	 *		template<int N_>
 	 *		auto operator = (const T (&val)[N_]) require_r(Class<T, N> &, (N != N_))
  	 *		{
	 *			...
	 *			return *this;
	 *		}
	 */
#define require_r(ret, ...) -> enable_if_t<(__VA_ARGS__), ret>

	/**
	 *	@brief
	 *		Used to perform SFINAE operations by the argument.
	 *		...	- parameters of the operation
	 *
	 *	Example:
	 *		template<class T, int N, int N_>
	 *		Class(const T (&val)[N_], require_p(N != N_)) 
	 *		{
	 *			...
	 *		}
	 */
#define require_p(...) enable_if_t<(__VA_ARGS__), void **> = nullptr

	/**
	 *	@brief
	 *		Used to perform SFINAE operations by the template argument.
	 *		...	- parameters of the operation
	 *
	 *	Example:
	 *		template<int N_, require(N != N_)>
	 *		Class<T, N> & operator = (const T (&val)[N_])
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 */
#define require(...) typename = enable_if_t<(__VA_ARGS__)>

	/**
	 *	@brief
	 *		Used to perform SFINAE operations by the template argument when
	 *		there are several declarations of such function.
	 *		Each declaration can be indexed by number in range 1..256
	 *		...	- parameters of the operation
	 *
	 *	Example:
	 *		template<typename T_,
	 *			require_i(1,
	 *				same_type(T_, String)
	 *				)>
 	 *		Class & operator = (const T_ & val)
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 */
#define require_i(index, ...) typename = enable_if_t<(__VA_ARGS__)>, pp_enum(index, pp_enum_prepend, int = )

	/**
	 *	@brief
	 *		Used to select types we need from a template function
	 *
	 *  Usage:
	 *		require(same_type(target, type))
	 *		require(based_on(target, base_type))
	 */
#define same_type(target, type)		is_same<decay_t<target>, type>::value
#define based_on(target, base)		is_base_of<base, decay_t<target>>::value

	/**
	 *	@brief
	 *		These things are quitely opposite from same_type, based_on etc.
	 */
#define not_same(target, par)		!is_same<decay_t<target>, par>::value
#define not_based_on(target, par)	!is_base_of<par, decay_t<target>>::value

#define can_construct(target, ...)	(Rapture::IsConstructible<target, __VA_ARGS__>::value || std::is_constructible<target, __VA_ARGS__>::value)
#define can_call(func, ...)			!std::is_empty<decltype(std::invoke(func, __VA_ARGS__))>::value

#define get_(i) get<i>()
#define set_(i, ...) set<i>(__VA_ARGS__)

//---------------------------------------------------------------------------
#endif
