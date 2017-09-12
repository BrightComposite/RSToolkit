//---------------------------------------------------------------------------

#pragma once

#ifndef BOOL_TYPES_H
#define BOOL_TYPES_H

//---------------------------------------------------------------------------

#include "sfinae.h"

//---------------------------------------------------------------------------

namespace asd
{
	template<bool ... V>
	struct is_true : false_type {};
	
	template<bool ... V>
	struct is_true<true, V...> : is_true<V...> {};
	
	template<bool ... V>
	struct is_true<false, V...> : false_type {};
	
	template<>
	struct is_true<> : true_type {};
	
	template<bool ... V>
	struct is_false : false_type {};
	
	template<bool ... V>
	struct is_false<false, V...> : true_type {};
	
	template<bool ... V>
	struct is_false<true, V...> : is_false<V...> {};
	
	template<>
	struct is_false<> : false_type {};

//---------------------------------------------------------------------------
	
	template<typename T, typename U>
	struct are_same : is_same<T, U> {};
	
	template<typename ... T, typename ... U>
	struct are_same<tuple<T...>, tuple<U...>> : is_same<tuple<decay_t<T>...>, tuple<decay_t<U>...>> {};
	
	template<typename T, typename ... U>
	struct are_same<T, tuple<U...>> : is_same<tuple<T>, tuple<decay_t<U>...>> {};
	
	template<typename U, typename ... T>
	struct are_same<tuple<T...>, U> : is_same<tuple<decay_t<T>...>, tuple<U>> {};
	
	template<typename ... T, typename ... U>
	struct are_same<types<T...>, types<U...>> : is_same<types<decay_t<T>...>, types<decay_t<U>...>> {};
	
	template<typename T, typename ... U>
	struct are_same<T, types<U...>> : is_same<types<T>, types<decay_t<U>...>> {};
	
	template<typename U, typename ... T>
	struct are_same<types<T...>, U> : is_same<types<decay_t<T>...>, types<U>> {};
	
	template<typename From, typename To>
	struct are_convertible : is_convertible<From, To> {};
	
	namespace internals
	{
		template<typename T, typename U, bool>
		struct are_convertible : false_type {};
		
		template<template<class...> class Tpl, typename ... T, typename ... U>
		struct are_convertible<Tpl<T...>, Tpl<U...>, true> : is_true<is_convertible<T, U>::value...> {};
	}
	
	template<template<class...> class Tpl, typename ... T, typename ... U>
	struct are_convertible<Tpl<T...>, Tpl<U...>>
		: internals::are_convertible<Tpl<T...>, Tpl<U...>, sizeof...(T) == sizeof...(U)>
	{
	};
	
//---------------------------------------------------------------------------

#define bool_struct(name, ...)                            \
    struct name                                                    \
    {                                                            \
        static const bool value = is_true<__VA_ARGS__>::value;    \
    }

//---------------------------------------------------------------------------
	
	/**
	 *	@brief
	 *		Used to select types we need from a template function
	 *
	 *  Usage:
	 *		useif<same_type<target, type>::value>
	 *		useif<based_on<target, base_type>::value>
	 *		useif<same_types<tuple<target>, tuple<types>>::value>
	 */
	template<class T, class U>
	bool_struct(same_type, is_same<decay_t<T>, decay_t<U>>::value);
	template<class T, class U>
	bool_struct(based_on, is_base_of<decay_t<U>, decay_t<T>>::value);
	template<class T, class U>
	bool_struct(same_types, are_same<decay_t<T>, decay_t<U>>::value);
	
	/**
	 *	@brief
	 *		These things are quitely opposite to same_type, based_on etc.
	 */
	template<class T, class U>
	bool_struct(not_same_type, !is_same<decay_t<T>, decay_t<U>>::value);
	template<class T, class U>
	bool_struct(not_based_on, !is_base_of<decay_t<U>, decay_t<T>>::value);
	template<class T, class U>
	bool_struct(not_same_types, !are_same<decay_t<T>, decay_t<U>>::value);

//---------------------------------------------------------------------------
	
	/**
	 *	Used instead of std::is_constructible to allow "friendship"
	 */
	template<class T, class ... A>
	struct is_constructible
	{
	private:
		template<typename U>
		static true_type _(sfinae_int<decltype(U(declval<A>()...))>);
		template<typename U>
		static false_type _(...);
	
	public:
		static constexpr bool value = identity<decltype(_<T>(0))>::type::value;
	};
	
	/**
	 *	Used instead of std::is_constructible to allow list-initialization
	 */
	template<class T, class ... A>
	struct is_list_constructible
	{
	private:
		template<typename U>
		static true_type _(sfinae_int<decltype(U {declval<A>()...})>);
		template<typename U>
		static false_type _(...);
	
	public:
		static constexpr bool value = identity<decltype(_<T>(0))>::type::value;
	};

//---------------------------------------------------------------------------
	
	template<class T>
	bool_struct(is_determined_class, !std::is_class<T>::value || std::is_base_of<T, T>::value);
	template<class T, class ... A>
	bool_struct(can_construct, is_constructible<T, A...>::value || is_list_constructible<T, A...>::value);
	template<class T, class ... A>
	bool_struct(cant_construct, !can_construct<T, A...>::value);

#define flag_class(name, checker)    \
    struct name {};                    \
                                    \
    template<class T>                \
    bool_struct(checker,    \
        is_base_of<name, T>::value    \
    )
}

//---------------------------------------------------------------------------
#endif
