//---------------------------------------------------------------------------

#pragma once

#ifndef META_SFINAE_H
#define META_SFINAE_H

//---------------------------------------------------------------------------

#include "Types.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class T>
	struct sfinae_int { sfinae_int(int) {} };

//---------------------------------------------------------------------------

#define type_checker(name, type)						                                                                    \
	template<class T>											                                                            \
	struct name						                                                                                        \
	{						                                                                                                \
	private:						                                                                                        \
		template<typename U>						                                                                        \
		static true_type  _(sfinae_int<typename U::type>);		                                                            \
		template<typename U>						                                                                        \
		static false_type _(...);						                                                                    \
																                                                            \
	public:														                                                            \
		static constexpr bool value = decltype(_<T>(0))::value;	                                                            \
	};

#define function_checker(name, func)							                                                            \
	template<class ... A>										                                                            \
	struct name						                                                                                        \
	{						                                                                                                \
	private:						                                                                                        \
		template<typename ... B>						                                                                    \
		static true_type  _(sfinae_int<decltype(func(declval<B>()...))>);	                                                \
		template<typename ... B>						                                                                    \
		static false_type _(...);						                                                                    \
																                                                            \
	public:														                                                            \
		static constexpr bool value = decltype(_<A...>(0))::value;	                                                        \
	};

#define method_checker(name, method)					                                                                    \
	template<class T, class ... A>										                                                    \
	struct name						                                                                                        \
	{						                                                                                                \
	private:						                                                                                        \
		template<typename U>						                                                                        \
		static true_type  _(sfinae_int<decltype(declval<U>().method(declval<A>()...))>);			                        \
		template<typename U>						                                                                        \
		static false_type _(...);						                                                                    \
																                                                            \
	public:														                                                            \
		static constexpr bool value = decltype(_<T>(0))::value;	                                                            \
	};

#define static_method_checker(name, method)					                                                                \
	template<class T, class ... A>										                                                    \
	struct name						                                                                                        \
	{						                                                                                                \
	private:						                                                                                        \
		template<typename U>						                                                                        \
		static true_type  _(sfinae_int<decltype(U::method(declval<A>()...))>);											    \
		template<typename U>						                                                                        \
		static false_type _(...);						                                                                    \
																                                                            \
	public:														                                                            \
		static constexpr bool value = decltype(_<T>(0))::value;	                                                            \
	};

#define member_checker(name, member)					                                                                    \
	template<class T>											                                                            \
	struct name						                                                                                        \
	{						                                                                                                \
	private:						                                                                                        \
		template<typename U>						                                                                        \
		static true_type  _(sfinae_int<decltype(T::member)>);		                                                        \
		template<typename U>						                                                                        \
		static false_type _(...);						                                                                    \
																                                                            \
	public:														                                                            \
		static constexpr bool value = decltype(_<T>(0))::value;	                                                            \
	};

//---------------------------------------------------------------------------

	template<class T, class ... A>
	struct is_callable
	{
	private:
		template<typename U>
		static true_type  _(sfinae_int<decltype(declval<U>()(declval<A>()...))>);
		template<typename U>
		static false_type _(...);

	public:
		static constexpr bool value = decltype(_<T>(0))::value;
	};

	template<class T>
	struct has_caller
	{
	private:
		template<typename U>
		static true_type  _(sfinae_int<decltype(&U::operator())>);
		template<typename U>
		static false_type _(...);

	public:
		static constexpr bool value = decltype(_<T>(0))::value;
	};

	template<class R, class F, class ... A>
	using can_transform_to = is_convertible<decltype(declval<F>()(declval<A>()...)), R>;

//---------------------------------------------------------------------------

#define type_getter(name, T, tgt, def)					                                                                    \
	template<class T>									                                                                    \
	struct name											                                                                    \
	{													                                                                    \
	private:											                                                                    \
		template<class U>								                                                                    \
		static auto _(int) -> identity<typename U::tgt>;                                                                    \
		template<class U>								                                                                    \
		static auto _(...) -> identity<def>;			                                                                    \
														                                                                    \
		using identitype = decltype(_<T>(0));			                                                                    \
														                                                                    \
	public:												                                                                    \
		using type = typename identitype::type;			                                                                    \
	}
}

//---------------------------------------------------------------------------
#endif
