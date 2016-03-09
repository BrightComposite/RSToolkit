//---------------------------------------------------------------------------

#ifndef META_H
#define META_H

//---------------------------------------------------------------------------

#include "Types.h"
#include "Require.h"

#include <tuple>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::tuple;

	namespace Meta
	{
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
		 *  (of course, it is the Tuple class)
		 */

		template<int I, typename H, typename ... T>
		struct TypeCutter
		{
			typedef typename TypeCutter<I - 1, T...>::Type Type;
		};

		template<int I, typename ... T>
		struct TypeCutter<I, Tuple<T...>>
		{
			typedef typename TypeCutter<I, T...>::Type Type;
		};

		template <typename H, typename ... T>
		struct TypeCutter<0, H, T...>
		{
			typedef Tuple<H, T...> Type;
		};

		template <int I, typename H, typename ... T>
		using cut_t = typename TypeCutter<I, H, T...>::Type;

		template <int I, typename H, typename ... T>
		using extract_t = typename cut_t<I, H, T...>::Head;

		/**
		 *  @brief
		 *  Returns tuple <S ...> with N elements of type T
		 */

		template<int N, typename T, typename ... S>
		struct Expander
		{
			typedef typename Expander<N - 1, T, decay_t<T>, S...>::Type Type;
		};

		template <typename T, typename ... S>
		struct Expander<0, T, S...>
		{
			typedef tuple<S...> Type;
		};

		template<int I, typename H, typename ... S>
		using expand_t = typename Expander<I, H, S...>::Type;

		/**
		*  @brief
		*  Unites some types. If type is the tuple, it expands.
		*/

		template<typename ... S>
		struct Uniter
		{
			typedef tuple<S...> Type;
		};

		/**
		*  @brief
		*  Returns result of Uniter's work.
		*/

		template<typename ... S>
		using union_t = typename Uniter<S...>::Type;

		template<typename T, typename ... S>
		struct TupleCombiner {};

		template <typename ... T, typename ... S>
		struct TupleCombiner<tuple<T...>, tuple<S...>>
		{
			typedef tuple<T..., S...> Type;
		};

		template<typename ... T, typename ... S>
		struct Uniter<tuple<T...>, S...>
		{
			typedef typename TupleCombiner<union_t<T...>, union_t<S...>>::Type Type;
		};

		/**
		 *  @brief
		 *  Finder implementation.
		 *  Seeks for the type T in template-pack.
		 *  Returns pos of type in set if it was found, and -1 otherwise.
		 */

		template<int R, typename T, typename H, typename ... S>
		struct FinderImpl
		{
			static const int result = FinderImpl<R + 1, T, S...>::result;
		};

		template<int R, typename T, typename ... S>
		struct FinderImpl<R, T, T, S ...>
		{
			static const int result = R;
		};

		template<int R, typename T, typename H>
		struct FinderImpl<R, T, H>
		{
			static const int result = -1;
		};

		/**
		 *  @brief
		 *  Wraps FinderImpl to initialize counter.
		 *  Also provides tuple support.
		 */

		template<typename T, typename H, typename ... S>
		struct Finder
		{
			static const int result = FinderImpl<0, T, H, S...>::result;
		};

		template<typename T, typename H, typename ... S>
		struct Finder<T, tuple<H, S...>>
		{
			static const int result = FinderImpl<0, T, H, S...>::result;
		};

		template<typename T, typename H, typename ... S>
		using find_t = Finder<T, H, S...>;

		/**
		 *  @brief
		 *  BaseFinder implementation.
		 *  Quite the same thing as FinderImpl but it searches for base of
		 *  target class.
		 *  Returns pos of type in set if it was found, and -1 otherwise.
		 */

		template <int R, bool isBase, typename T, typename ... A>
		struct BaseFinderImpl {};

		template<int R, typename T, typename H, typename ... S>
		struct BaseFinderImpl<R, false, T, H, S...>
		{
			static const int result = BaseFinderImpl<R + 1, is_base_of<H, T>::value, T, S...>::result;
		};

		template<int R, typename T, typename ... A>
		struct BaseFinderImpl<R, true, T, A ...>
		{
			static const int result = R;
		};

		template<int R, typename T>
		struct BaseFinderImpl<R, false, T>
		{
			static const int result = -1;
		};

		/**
		 *  @brief
		 *  Wraps BaseFinderImpl to initialize counter.
		 *  Also provides tuple support.
		 */

		template<typename T, typename H, typename ... S>
		struct BaseFinder
		{
			static const int result = BaseFinderImpl<0, is_base_of<H, T>::value, T, S...>::result;
		};

		template<typename T, typename H, typename ... S>
		struct BaseFinder<T, tuple<H, S...>>
		{
			static const int result = BaseFinderImpl<0, is_base_of<H, T>::value, T, S...>::result;
		};

		/**
		 *	@brief
		 *  Seeks for the type T in <A...> .
		 *  Returns true_type if it was found and false_type otherwise.
		 *	T - target type, A... - set of keys, H - first element of set.
		 */

		template<typename T, typename ... A>
		struct HasType {};

		template<typename T, typename H, typename ... A>
		struct HasType<T, H, A...>
		{
			typedef typename HasType<T, A...>::Type Type;
		};

		template<typename T, typename ... A>
		struct HasType<T, T, A...>
		{
			typedef true_type Type;
		};

		template<typename T>
		struct HasType<T>
		{
			typedef false_type Type;
		};

		template<typename T, typename ... A>
		using has_type = typename HasType<T, A...>::Type;

		/**
		 *  @brief
		 *  Seeks for the type T in <K> and returns assosiated type from <V>.
		 *  Returns the Empty type if T was not found.
		 *	T - target key, K - set of keys, V - set of values.
		 *  Each set is represented by the tuple type.
		 */

		template<typename T, class K, class V>
		struct TypeFinder {};

		template<typename T, typename KH, typename VH, typename ... KT, typename ... VT>
		struct TypeFinder<T, tuple<KH, KT...>, tuple<VH, VT...>>
		{
			typedef typename TypeFinder<T, tuple<KT...>, tuple<VT...>>::Type Type;
		};

		template<typename T, typename VH, typename ... KT, typename ... VT>
		struct TypeFinder<T, tuple<T, KT...>, tuple<VH, VT...>>
		{
			typedef VH Type;
		};

		template<typename T>
		struct TypeFinder<T, tuple<>, tuple<>>
		{
			typedef Empty Type;
		};

		template<typename T, typename K, typename V>
		using find_type_t = typename TypeFinder<T, K, V>::Type;

		/**
		 *	Used instead of std::is_constructible to allow "friendship"
		 */
		template <class T, class ... A>
		struct IsConstructible
		{
			template<typename T_, typename = decltype(T_(declval<A>()...))>
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
		};

		struct Initialize
		{
			template<typename T>
			static inline auto init(T & target, const T & value) require_r(void, !is_array<T>::value)
			{
				target = value;
			}

			template<typename T, size_t N>
			static inline void init(T(&target)[N], const T(&value)[N])
			{
				ArrayInitialize<N>::init(target, value);
			}

			template<typename T, size_t N, size_t N_>
			static void init(T(&target)[N], const T(&value)[N_], T def = 0, require_p(N != N_))
			{
				register size_t i = 0;

				for(; i < N_ && i < N; ++i)
					target[i] = value[i];

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
		struct is_true<>
		{
			static const bool value = true;
		};

		template<bool ...>
		struct is_false {};

		template<bool H, bool ... T>
		struct is_false<H, T...>
		{
			static const bool value = !H && is_false<T...>::value;
		};

		template<>
		struct is_false<>
		{
			static const bool value = true;
		};

		template<typename T, typename ... A>
		struct are_same
		{
			static const bool value = false;
		};

		template<typename T>
		struct are_same<T, T>
		{
			static const bool value = true;
		};
	}

	using namespace Meta;

//---------------------------------------------------------------------------

#define sfinae_checker(name, args, expression)						 \
	template<__expand args>											 \
	struct name														 \
	{																 \
		template<class T_, typename = decltype(__expand expression)> \
		static auto fn(int) -> true_type;							 \
																	 \
		template<class T_>											 \
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