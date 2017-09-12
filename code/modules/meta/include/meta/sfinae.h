//---------------------------------------------------------------------------

#pragma once

#ifndef META_SFINAE_H
#define META_SFINAE_H

//---------------------------------------------------------------------------

#include "types.h"

//---------------------------------------------------------------------------

namespace asd
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
		static constexpr bool value = identity_t<decltype(_<T>(0))>::value;	                                                            \
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
		static constexpr bool value = identity_t<decltype(_<A...>(0))>::value;	                                                        \
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
		static constexpr bool value = identity_t<decltype(_<T>(0))>::value;	                                                            \
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
		static constexpr bool value = identity_t<decltype(_<T>(0))>::value;	                                                            \
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
		static constexpr bool value = identity_t<decltype(_<T>(0))>::value;	                                                            \
	};

#define operator_checker(name, op)					                                                                    	\
	template<class T>										                                                    			\
	struct name						                                                                                        \
	{						                                                                                                \
	private:						                                                                                        \
		template<typename U>						                                                                        \
		static true_type  _(sfinae_int<decltype(declval<U>() op declval<U>())>);			                        		\
		template<typename U>						                                                                        \
		static false_type _(...);						                                                                    \
																                                                            \
	public:														                                                            \
		static constexpr bool value = identity_t<decltype(_<T>(0))>::value;	                                                            \
	};
	
	
	operator_checker(is_less_comparable, <)
	operator_checker(is_equal_comparable, ==)
	
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
		static constexpr bool value = identity_t<decltype(_<T>(0))>::value;
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
		static constexpr bool value = identity_t<decltype(_<T>(0))>::value;
	};

	template<class R, class F, class ... A>
	using can_transform_to = is_convertible<decltype(declval<F>()(declval<A>()...)), R>;

	method_checker(can_clone, clone);
	method_checker(is_pointerable, operator->);

//---------------------------------------------------------------------------

#define type_getter(name, T, tgt, def)					                                                                    \
	template<class T>									                                                                    \
	struct name											                                                                    \
	{													                                                                    \
	private:											                                                                    \
		template<class U>								                                                                    \
		static auto _(int) -> typename U::tgt;                                                                    			\
		template<class U>								                                                                    \
		static auto _(...) -> def;			                                                                    			\
														                                                                    \
	public:																													\
		using type = identity_t<decltype(_<T>(0))>;	                                                						\
	}
}

//---------------------------------------------------------------------------
#endif
