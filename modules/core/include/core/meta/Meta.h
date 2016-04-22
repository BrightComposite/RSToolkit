//---------------------------------------------------------------------------

#ifndef META_H
#define META_H

//---------------------------------------------------------------------------

#include "Types.h"
#include "UseIf.h"

#include <tuple>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::tuple;

	template<typename T>
	struct Type
	{
		Type() {}
		Type(nullptr_t) {}
	};

#define gettype(...) Type<__VA_ARGS__>()

	/**
		*  @brief
		*  Used by other meta-structures to
		*  get type of generation I of some variadic-templated class
		*/

	template<int I, typename ... T>
	struct TypeCutter {};

	template<int I, typename H, typename ... T>
	struct TypeCutter<I, H, T...> : TypeCutter<I - 1, T...> {};

	template<typename H, typename ... T>
	struct TypeCutter<0, H, T...>
	{
		typedef H head;
		typedef tuple<H, T...> type;
	};

	template<int I>
	struct TypeCutter<I>
	{
		typedef Empty head;
		typedef Empty type;
	};

	template <int I, typename ... T>
	using cut_t = typename TypeCutter<I, T...>::type;

	template <int I, typename ... T>
	using extract_t = typename TypeCutter<I, T...>::head;

	template<typename ... T>
	using first_t = extract_t<0, T...>;

	template<typename ... T>
	using last_t = extract_t<sizeof...(T) - 1, T...>;

	/**
		*  @brief
		*  Returns tuple <S ...> with N elements of type T
		*/

	template<int N, typename T, typename ... S>
	struct Expander : Expander<N - 1, T, decay_t<T>, S...> {};

	template <typename T, typename ... S>
	struct Expander<0, T, S...>
	{
		typedef tuple<S...> type;
	};

	template<int I, typename H, typename ... S>
	using expand_t = typename Expander<I, H, S...>::type;

	/**
		*  @brief
		*  Seeks for the type T in template-pack.
		*  Returns pos of type in set if it was found, and -1 otherwise.
		*/

	namespace Internals
	{
		template<int R, typename T, typename H, typename ... S>
		struct find_t0 : find_t0<R + 1, T, S...> {};

		template<int R, typename T, typename ... S>
		struct find_t0<R, T, T, S ...> : std::integral_constant<int, R> {};

		template<int R, typename T, typename H>
		struct find_t0<R, T, H> : std::integral_constant<int, -1> {};

	}

	template<typename T, typename H, typename ... S>
	struct find_t : Internals::find_t0<0, T, H, S...> {};

	template<typename T, typename H, typename ... S>
	struct find_t<T, tuple<H, S...>> : Internals::find_t0<0, T, H, S...> {};

	/**
		*	@brief
		*  Seeks for the type T in <A...> .
		*  Returns true_type if it was found and false_type otherwise.
		*	T - target type, A... - set of keys, H - first element of set.
		*/

	template<typename T, typename ... A>
	struct has_type {};

	template<typename T, typename H, typename ... A>
	struct has_type<T, H, A...> : has_type<T, A...> {};

	template<typename T, typename ... A>
	struct has_type<T, T, A...> : true_type {};

	template<typename T>
	struct has_type<T> : false_type {};

	/**
		*	@brief
		*  Iterates through the tuple types with the Functor.
		*/

	template<class Type>
	struct foreach_t {};

	template<class TH, class ... TT>
	struct foreach_t<tuple<TH, TT ...>>
	{
		template<typename Functor, class ... A>
		static inline void iterate(A &&... args)
		{
			Functor::iterate<TH>(forward<A>(args)...);
			foreach_t<tuple<TT ...>>::iterate(forward<A>(args)...);
		}
	};

	template <>
	struct foreach_t<tuple<>>
	{
		template <typename Functor, class ... A>
		static inline void iterate(A &&...) {}
	};

	/**
		*	Used instead of std::is_constructible to allow "friendship"
		*/
	template <class T, class ... A>
	struct IsConstructible
	{
		template<typename U, typename = decltype(U(declval<A>()...))>
		static true_type test(int);

		template<typename>
		static false_type test(...);

		static const bool value = decltype(test<T>(0))::value;
		typedef tuple<A...> args_type;
	};

	template<size_t N>
	struct ArrayInitialize
	{
		template<typename T>
		static inline void init(T(&target)[N], const T(&value)[N])
		{
			for(register size_t i = 0; i < N; ++i)
				target[i] = value[i];
		}
	};

	template<>
	struct ArrayInitialize<1>
	{
		template<typename T>
		static inline void init(T(&target)[1], const T(&value)[1])
		{
			target[0] = value[0];
		}
	};

	template<>
	struct ArrayInitialize<2>
	{
		template<typename T>
		static inline void init(T(&target)[2], const T(&value)[2])
		{
			target[0] = value[0];
			target[1] = value[1];
		}
	};

	template<>
	struct ArrayInitialize<3>
	{
		template<typename T>
		static inline void init(T(&target)[3], const T(&value)[3])
		{
			target[0] = value[0];
			target[1] = value[1];
			target[2] = value[2];
		}
	};

	template<>
	struct ArrayInitialize<4>
	{
		template<typename T>
		static inline void init(T(&target)[4], const T(&value)[4])
		{
			target[0] = value[0];
			target[1] = value[1];
			target[2] = value[2];
			target[3] = value[3];
		}

		template<typename T>
		static inline void init(T(&target)[4], T(&&value)[4])
		{
			target[0] = move(value[0]);
			target[1] = move(value[1]);
			target[2] = move(value[2]);
			target[3] = move(value[3]);
		}
	};

	struct Initialize
	{
		template<typename T, skipif(is_array<T>::value)>
		static inline void init(T & target, const T & value)
		{
			target = value;
		}

		template<typename T, skipif(is_array<T>::value)>
		static inline void init(T & target, T && value)
		{
			target = forward<T>(value);
		}

		template<typename T, size_t N>
		static inline void init(T(&target)[N], const T(&value)[N])
		{
			ArrayInitialize<N>::init(target, value);
		}

		template<typename T, size_t N>
		static inline void init(T(&target)[N], T(&&value)[N])
		{
			ArrayInitialize<N>::init(target, forward<array_t<T, N>>(value));
		}

		template<typename T, size_t N, size_t N_, useif(N != N_)>
		static void init(T(&target)[N], const T(&value)[N_], T def = 0)
		{
			register size_t i = 0;

			for(; i < N_ && i < N; ++i)
				target[i] = value[i];

			for(; i < N; ++i)
				target[i] = def;
		}

		template<typename T, size_t N, size_t N_, useif(N != N_)>
		static void init(T(&target)[N], T(&&value)[N_], T def = 0)
		{
			register size_t i = 0;

			for(; i < N_ && i < N; ++i)
				target[i] = move(value[i]);

			for(; i < N; ++i)
				target[i] = def;
		}

		template<typename T, size_t N>
		static inline void init(T(&target)[N], T filler)
		{
			for(register size_t i = 0; i < N; ++i)
				target[i] = filler;
		}

		template<typename T, size_t N>
		static void init(T(&target)[N], const initializer_list<T> & value, T def = 0)
		{
			register size_t i = 0;
			const auto ex = value.begin();

			for(; i < value.size() && i < N; ++i)
				target[i] = ex[i];

			for(; i < N; ++i)
				target[i] = def;
		}
	};

	template<typename T>
	struct array_size
	{
		static const size_t value = 0;
	};

	template<typename T, size_t N>
	struct array_size<T[N]>
	{
		static const size_t value = N;
	};

	template<bool ...>
	struct is_true {};

	template<bool H, bool ... T>
	struct is_true<H, T...>
	{
		static const bool value = H && is_true<T...>::value;
	};

	template<>
	struct is_true<> : true_type {};

	template<bool ...>
	struct is_false {};

	template<bool H, bool ... T>
	struct is_false<H, T...>
	{
		static const bool value = !H && is_false<T...>::value;
	};

	template<>
	struct is_false<> : false_type {};

	template<typename T, typename U>
	struct are_same : is_same<T, U> {};

	template<typename ... T, typename ... U>
	struct are_same<tuple<T...>, tuple<U...>> : is_same<tuple<decay_t<T>...>, tuple<decay_t<U>...>> {};

//---------------------------------------------------------------------------

#define sfinae_checker(name, args, expression)						 \
	template<pp_expand args>											 \
	struct name														 \
	{																 \
		template<class U, typename = decltype(pp_expand expression)> \
		static auto fn(int) -> true_type;							 \
																	 \
		template<class U>											 \
		static auto fn(...) -> false_type;							 \
																	 \
		typedef decltype(fn<Empty>(0)) type;						 \
		static const bool value = type::value;						 \
	};

#define function_checker(name, func)			\
	sfinae_checker(								\
		name,									\
		(class ... A),							\
		(func(declval<A>()...))					\
	)											\

#define method_checker(name, method)			\
	sfinae_checker(								\
		name,									\
		(class T, class ... A),					\
		(declval<T>().method(declval<A>()...))	\
	)

#define member_checker(name, member)			\
	sfinae_checker(								\
		name,									\
		(class T),								\
		(T::member)								\
	)											\

//---------------------------------------------------------------------------

	method_checker(is_functor, operator())
}

//---------------------------------------------------------------------------
#endif