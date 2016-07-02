//---------------------------------------------------------------------------

#pragma once

#ifndef META_H
#define META_H

//---------------------------------------------------------------------------

#include "Adapt.h"
#include "BoolTypes.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace Internals
	{
		template<int I, typename ... T>
		struct cut_t {};

		template<int I, typename H, typename ... T>
		struct cut_t<I, H, T...> : cut_t<I - 1, T...> {};

		template<typename H, typename ... T>
		struct cut_t<0, H, T...>
		{
			typedef H head;
			typedef tuple<H, T...> type;
		};

		template<int I>
		struct cut_t<I>
		{
			typedef Empty head;
			typedef Empty type;
		};

		template<template<typename...> class Tpl, int I, typename ... T>
		struct cut_t<I, Tpl<T...>> : cut_t<I, T...> {};
	}

	/**
	 *  @brief
	 *  Used by other meta-structures to
	 *  get type of generation I of some variadic-templated class
	 */
	template <int I, typename ... T>
	using cut_t = typename Internals::cut_t<I, T...>::type;

	template <int I, typename ... T>
	using extract_t = typename Internals::cut_t<I, T...>::head;

	template<typename ... T>
	using first_t = extract_t<0, T...>;

	template<typename ... T>
	using last_t = extract_t<sizeof...(T) - 1, T...>;

	namespace Internals
	{
		template<int N, typename T, typename ... S>
		struct expand_t : expand_t<N - 1, T, decay_t<T>, S...> {};

		template <typename T, typename ... S>
		struct expand_t<0, T, S...>
		{
			typedef tuple<S...> type;
		};
	}

	/**
	 *  @brief
	 *  Returns tuple<S ...> with N elements of type T
	 */
	template<int I, typename H, typename ... S>
	using expand_t = typename Internals::expand_t<I, H, S...>::type;

	namespace Internals
	{
		template<int R, typename T, typename H, typename ... S>
		struct find_t : find_t<R + 1, T, S...> {};

		template<int R, typename T, typename ... S>
		struct find_t<R, T, T, S ...> : std::integral_constant<int, R> {};

		template<int R, typename T, typename H>
		struct find_t<R, T, H> : std::integral_constant<int, -1> {};
	}

	/**
	 *  @brief
	 *  Seeks for the type T in template-pack.
	 *  Returns pos of type in set if it was found, and -1 otherwise.
	 */
	template<typename T, typename H, typename ... S>
	struct find_t : Internals::find_t<0, T, H, S...> {};

	/**
	 *  @brief
	 *  Seeks for the type T in template-pack.
	 *  Returns pos of type in set if it was found, and -1 otherwise.
	 */
	template<template<typename...> class Tpl, typename T, typename H, typename ... S>
	struct find_t<T, Tpl<H, S...>> : Internals::find_t<0, T, H, S...> {};

	/**
	 *	@brief
	 *  Seeks for the type T in <A...> .
	 *  Returns true_type if it was found and false_type otherwise.
	 *	T - target type, A... - set of keys, H - first element of set.
	 */
	template<typename T, typename ... A>
	struct has_type {};

	/**
	 *	@brief
	 *  Seeks for the type T in <A...> .
	 *  Returns true_type if it was found and false_type otherwise.
	 *	T - target type, A... - set of keys, H - first element of set.
	 */
	template<typename T, typename H, typename ... A>
	struct has_type<T, H, A...> : has_type<T, A...> {};

	/**
	 *	@brief
	 *  Seeks for the type T in <A...> .
	 *  Returns true_type if it was found and false_type otherwise.
	 *	T - target type, A... - set of keys, H - first element of set.
	 */
	template<typename T, typename ... A>
	struct has_type<T, T, A...> : true_type {};

	/**
	 *	@brief
	 *  Seeks for the type T in <A...> .
	 *  Returns true_type if it was found and false_type otherwise.
	 *	T - target type, A... - set of keys, H - first element of set.
	 */
	template<typename T>
	struct has_type<T> : false_type {};

//---------------------------------------------------------------------------

	/**
	 *	@brief
	 *  Iterates through the tuple types with the Functor.
	 */
	template<class ... A>
	struct foreach_t {};

	/**
	 *	@brief
	 *  Iterates through the tuple types with the Functor.
	 */
	template<class ... A>
	struct foreach_t<tuple<A...>> : foreach_t<A...> {};

	/**
	 *	@brief
	 *  Iterates through the tuple types with the Functor.
	 */
	template<class H, class ... T>
	struct foreach_t<H, T ...>
	{
		template<typename Functor, bool Cond, class ... A, useif <
			is_same<decltype(Functor::template iterate<H>(declval<A>()...)), bool>::value
			> endif
		>
		static inline bool iterate(A &&... args)
		{
			if(Functor::template iterate<H>(forward<A>(args)...) == !Cond)
				return !Cond;

			return foreach_t<T...>::template iterate<Functor, Cond>(forward<A>(args)...);
		}

		template<typename Functor, class ... A, useif <
			!is_empty<decltype(Functor::template iterate<H>(declval<A>()...))>::value
			> endif
		>
		static inline void iterate(A &&... args)
		{
			Functor::template iterate<H>(forward<A>(args)...);
			foreach_t<T...>::template iterate<Functor>(forward<A>(args)...);
		}
	};
	
	/**
	 *	@brief
	 *  Iterates through the tuple types with the Functor.
	 */
	template <>
	struct foreach_t<>
	{
		template<typename Functor, bool Cond, class ... A>
		static inline bool iterate(A &&...)
		{
			return Cond;
		}

		template <typename Functor, class ... A>
		static inline void iterate(A &&...) {}
	};

	template<class From, class To>
	struct Cast {};

	sfinae_checker(can_cast, (class From, class To), (From, To),
		decltype(Cast<From, To>::cast(declval<To>(), declval<From>()))
	);
}

//---------------------------------------------------------------------------
#endif
