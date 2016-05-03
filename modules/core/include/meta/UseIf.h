//---------------------------------------------------------------------------

#ifndef META_USEIF_H
#define META_USEIF_H

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class SfinaeClass, bool ... Test>
	struct use_filter_t {};
}

#define declare_sfinae_class(Class)														\
struct Class																			\
{																						\
	enum type { value };																\
																						\
	template<bool ... Test, typename = Rapture::use_filter_t<Class, Test...>::type>		\
	static constexpr type filter()														\
	{																					\
		return type(0);																	\
	}																					\
};																						\

declare_sfinae_class(SfinaeUse)
declare_sfinae_class(SfinaeSkip)

template<int I>
declare_sfinae_class(SfinaeSelect)


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
	 *	Example:
	 *		template<int N_, useif <N != N_> endif>
	 *		Class<T, N> & operator = (const T (&val)[N_])
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *
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
#define useif  useif_t  = SfinaeUse::filter
#define skipif skipif_t = SfinaeSkip::filter

	/**
	 *	@brief
	 *		Used to perform SFINAE operations by the template argument when
	 *		there are several declarations of such function.
	 *		Each declaration can be indexed by any unique integer value
	 *		...	- parameters of the operation
	 *
	 *	Example:
	 *		template<typename U, selectif(0) <same_type<U, String>::value> endif>
	 *		Class & operator = (const U & val)
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 *
	 *	or
	 *		template<typename U, selectif(0) <same_type<U, String>::value> endif>
	 *		Class & operator = (const U & val);
	 *
	 *		template<typename U, selectif_t<0>>
	 *		Class & operator = (const U & val)
	 *		{
	 *			...
	 *			return *this;
	 *		}
	 */
#define selectif(I) selectif_t<I> = SfinaeSelect<I>::filter

#define endif ()

//---------------------------------------------------------------------------

namespace Rapture
{
	template<typename T, typename U>
	struct are_same : is_same<T, U> {};

	template<typename ... T, typename ... U>
	struct are_same<tuple<T...>, tuple<U...>> : is_same<tuple<decay_t<T>...>, tuple<decay_t<U>...>> {};

	template<typename T, typename ... U>
	struct are_same<T, tuple<U...>> : is_same<tuple<T>, tuple<decay_t<U>...>> {};

	template<typename U, typename ... T>
	struct are_same<tuple<T...>, U> : is_same<tuple<decay_t<T>...>, tuple<U>> {};

//---------------------------------------------------------------------------

#define declare_bool_struct(name, ...)			\
	struct name									\
	{											\
		static const bool value = __VA_ARGS__;	\
	};											\

	/**
	 *	@brief
	 *		Used to select types we need from a template function
	 *
	 *  Usage:
	 *		useif <same_type<target, type>::value> endif
	 *		useif <based_on<target, base_type>::value> endif
	 *		useif <same_types<tuple<target>, tuple<types>>::value> endif
	 */
	template<class T, class U>
	declare_bool_struct(same_type, is_same<decay_t<T>, decay_t<U>>::value)
	template<class T, class U>
	declare_bool_struct(based_on, is_base_of<decay_t<U>, decay_t<T>>::value)
	template<class T, class U>
	declare_bool_struct(same_types, are_same<decay_t<T>, decay_t<U>>::value)

	/**
	 *	@brief
	 *		These things are quitely opposite to same_type, based_on etc.
	 */
	template<class T, class U>
	declare_bool_struct(not_same_type, !is_same<decay_t<T>, decay_t<U>>::value)
	template<class T, class U>
	declare_bool_struct(not_based_on, !is_base_of<decay_t<U>, decay_t<T>>::value)
	template<class T, class U>
	declare_bool_struct(not_same_types, !are_same<decay_t<T>, decay_t<U>>::value)

	//---------------------------------------------------------------------------

	template<class T>
	struct identity
	{
		using type = T;
	};

	template<>
	struct use_filter_t<SfinaeUse> : identity<useif_t> {};

	template<bool ... Others>
	struct use_filter_t<SfinaeUse, true, Others...> : use_filter_t<SfinaeUse, Others...> {};

	template<int I>
	struct use_filter_t<SfinaeSelect<I>> : identity<selectif_t<I>> {};

	template<int I, bool ... Others>
	struct use_filter_t<SfinaeSelect<I>, true, Others...> : use_filter_t<SfinaeSelect<I>, Others...> {};

	template<bool ... Others>
	struct use_filter_t<SfinaeSkip, false, Others...> : identity<skipif_t> {};
}

//---------------------------------------------------------------------------
#endif
