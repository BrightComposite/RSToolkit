//---------------------------------------------------------------------------

#ifndef META_USEIF_H
#define META_USEIF_H

//---------------------------------------------------------------------------

namespace Rapture
{
	template<bool Test, class UseClass>
	struct use_filter {};

	enum class useif_t { Used };

	enum class skipif_t { Skipped };

	template<int index>
	struct selectcase_t
	{
		enum type { Selected };
	};

	template<>
	struct use_filter<true, useif_t>
	{
		static const auto value = useif_t::Used;
	};

	template<>
	struct use_filter<true, skipif_t>
	{
		static const auto value = skipif_t::Skipped;
	};

	template<int index>
	struct use_filter<true, selectcase_t<index>>
	{
		static const auto value = selectcase_t<index>::Selected;
	};

	template<int index>
	using selectif_t = typename selectcase_t<index>::type;

	/**
	 *	@brief
	 *		Used to perform SFINAE operations by the template argument.
	 *		...	- parameters of the operation
	 *
	 *	Example:
	 *		template<int N_, useif(N != N_)>
	 *		Class<T, N> & operator = (const T (&val)[N_])
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *
	 *	or
	 *		template<int N_, skipif(N == N_)>
	 *		Class<T, N> & operator = (const T (&val)[N_]);
	 *
	 *		template<int N_, skipif_t>
	 *		Class<T, N> & operator = (const T (&val)[N_])
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 */
#define useif(...) useif_t = use_filter<(__VA_ARGS__), useif_t>::value
#define skipif(...) skipif_t = use_filter<!(__VA_ARGS__), skipif_t>::value

	/**
	 *	@brief
	 *		Used to perform SFINAE operations by the template argument when
	 *		there are several declarations of such function.
	 *		Each declaration can be indexed by number in range 1..256
	 *		...	- parameters of the operation
	 *
	 *	Example:
	 *		template<typename U, selectif(0, same_type(U, String))>
	 *		Class & operator = (const U & val)
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *
	 *	or
	 *		template<typename U, selectif(0, same_type(U, String))>
	 *		Class & operator = (const U & val);
	 *
	 *		template<typename U, selectif_t<0>>
	 *		Class & operator = (const U & val)
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 */
#define selectif(index, ...) selectif_t<index> = use_filter<(__VA_ARGS__), selectcase_t<index>>::value

	/**
	 *	@brief
	 *		Used to select types we need from a template function
	 *
	 *  Usage:
	 *		useif(same_type(target, type))
	 *		useif(based_on(target, base_type))
	 *		useif(same_types((target), (types)))
	 */
#define same_type(target, type)		is_same<decay_t<target>, type>::value
#define based_on(target, base)		is_base_of<base, decay_t<target>>::value
#define same_types(target, types)	are_same<tuple<pp_expand target>, tuple<pp_expand types>>::value

	/**
	 *	@brief
	 *		These things are quitely opposite from same_type, based_on etc.
	 */
#define is_not_same(target, type)	!is_same<decay_t<target>, type>::value
#define is_not_based_on(target, base)	!is_base_of<base, decay_t<target>>::value
#define are_not_same(target, types)	!are_same<tuple<pp_expand target>, tuple<pp_expand types>>::value

#define can_construct(target, ...)	(Rapture::IsConstructible<target, __VA_ARGS__>::value || std::is_constructible<target, __VA_ARGS__>::value)
#define cant_construct(target, ...)	!can_construct(target, __VA_ARGS__)
#define can_call(func, ...)			!std::is_empty<decltype(std::invoke(func, __VA_ARGS__))>::value

#define get_(i) get<i>()
#define set_(i, ...) set<i>(__VA_ARGS__)
}

//---------------------------------------------------------------------------
#endif
