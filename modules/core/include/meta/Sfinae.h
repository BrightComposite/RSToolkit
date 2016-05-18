//---------------------------------------------------------------------------

#ifndef META_SFINAE_H
#define META_SFINAE_H

//---------------------------------------------------------------------------

#include "Types.h"

//---------------------------------------------------------------------------

namespace Rapture
{
#define sfinae_checker(name, a0, a1, ...)				\
	namespace Internals									\
	{													\
		template<class ... T>							\
		struct name {};									\
														\
		template<macrowrap a0>							\
		struct name<macrowrap a1>						\
		{												\
			template<class = int, class = __VA_ARGS__>	\
			static auto checker(int) -> true_type;		\
														\
			template<class = int>						\
			static auto checker(...) -> false_type;		\
		};												\
	}													\
														\
	template<macrowrap a0>								\
	struct name : decltype(								\
		Internals::name<macrowrap a1>					\
			::template checker<>(0)						\
	) {}												\

	//---------------------------------------------------------------------------

#define type_checker(name, type)						\
	sfinae_checker(										\
		name, (class T), (T),							\
		typename T::type								\
	)													\

#define function_checker(name, func)					\
	sfinae_checker(										\
		name, (class ... A), (A...),					\
		decltype(func(declval<A>()...))					\
	)													\

#define method_checker(name, method)					\
	sfinae_checker(										\
		name, (class T, class ... A), (T, A...),		\
		decltype(declval<T>().method(declval<A>()...))	\
	)

#define member_checker(name, member)					\
	sfinae_checker(										\
		name, (class T), (T),							\
		decltype(T::member)								\
	)													\

#define friend_sfinae(checker)							\
	template<class...>									\
	friend struct Rapture::Internals::checker			\

//---------------------------------------------------------------------------

	sfinae_checker(
		is_callable, (class T, class ... A), (T, A...),
		decltype(declval<T>()(declval<A>()...))
	);

//---------------------------------------------------------------------------

#define type_getter(name, param, tgt, def)				\
	template<class param>								\
	struct name											\
	{													\
	private:											\
		template<class U>								\
		static auto _(int) -> identity<typename U::tgt>;\
														\
		template<class U>								\
		static auto _(...) -> identity<def>;			\
														\
		typedef decltype(_<param>(0)) Decltype;			\
														\
	public:												\
		typedef typename Decltype::type type;			\
	}
}

//---------------------------------------------------------------------------
#endif
