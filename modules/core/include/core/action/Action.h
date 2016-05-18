//---------------------------------------------------------------------------

#ifndef ACTION_H
#define ACTION_H

//---------------------------------------------------------------------------

#include <functional>
#include <meta/UseIf.h>
#include <meta/Types.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::function;

#ifdef __GNUC__
#define invoke __invoke
#endif // __GNUC__

	typedef function<void()> Action;

	class ImmediateAction : public Action
	{
	public:
		template<class F, useif <
			is_same<void, decltype(std::invoke(declval<F>()))>::value
			> endif
		>
		ImmediateAction(F f) : Action(f) { operator()(); }

		template<class F, class ... A, useif <
			is_same<void, decltype(std::invoke(declval<F>(), declval<A>()...))>::value,
			(sizeof ... (A) > 0)
			> endif
		>
		ImmediateAction(F f, A &&... args) : Action(std::bind(f, forward<A>(args)...)) { operator()(); }
		~ImmediateAction() {}
	};

	class FinalAction : public Action
	{
	public:
		template<class F, useif <
			is_same<void, decltype(std::invoke(declval<F>()))>::value
			> endif
		>
		FinalAction(F f) : Action(f) {}

		template<class F, class ... A, useif <
			is_same<void, decltype(std::invoke(declval<F>(), declval<A>()...))>::value,
			(sizeof ... (A) > 0)
			> endif
		>
		FinalAction(F f, A &&... args) : Action(std::bind(f, forward<A>(args)...)) {}

		~FinalAction() { operator()(); }
	};

	template<typename R, typename ... A>
	using func_t = R (*)(A...);

	template<typename T, typename M>
	struct method_wrapper
	{
		typedef Empty MethodType;
	};

	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<const T, R(__thiscall C::*)(A...)>
	{
		typedef Empty MethodType;
	};

	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<const T *, R(__thiscall C::*)(A...)>
	{
		typedef Empty MethodType;
	};

	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T, R(__thiscall C::*)(A...)>
	{
		typedef R(__thiscall C::*MethodType)(A...);

		R operator()(A ... args)
		{
			return (object.*method)(args...);
		}

		T & object;
		MethodType method;
	};

	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T *, R(__thiscall C::*)(A...)>
	{
		typedef R(__thiscall C::*MethodType)(A...);

		R operator()(A ... args)
		{
			return (object->*method)(args...);
		}

		T * object;
		MethodType method;
	};

	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T, R(__thiscall C::*)(A...) const>
	{
		typedef R(__thiscall C::*MethodType)(A...) const;

		R operator()(A ... args)
		{
			return (object.*method)(forward<A>(args)...);
		}

		T & object;
		MethodType method;
	};

	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T *, R(__thiscall C::*)(A...) const>
	{
		typedef R(__thiscall C::*MethodType)(A...) const;

		R operator()(A ... args)
		{
			return (object->*method)(forward<A>(args)...);
		}

		T * object;
		MethodType method;
	};

	template<class T, class M>
	struct is_method : not_same_type<typename method_wrapper<T, M>::MethodType, Empty> {};

	template<class T, class M, useif <
		!std::is_pointer<T>::value,
		is_method<T, M>::value
		> endif
	>
	method_wrapper<T, M> wrap_method(T & object, M method)
	{
		return {object, method};
	}

	template<class T, class M, useif <
		is_method<T *, M>::value
		> endif
	>
	method_wrapper<T *, M> wrap_method(T * object, M method)
	{
		return {object, method};
	}

	namespace Internals
	{
		template<typename F, typename R, typename ... A, useif <
			std::is_convertible<F, func_t<R, A...>>::value
			> endif
		>
		func_t<R, A...> _lambda(F f, R(F::*mf) (A...) const)
		{
			return f;
		}

		template<typename F, typename R, typename ... A, useif <
			std::is_convertible<F, func_t<R, A...>>::value
			> endif
		>
		func_t<R, A...> _lambda(F f, R(F::*mf) (A...))
		{
			return f;
		}
	}

	/**
	 *	Works only for non-closure lambda
	 */
	template<typename F>
	auto lambda(F f) -> decltype(Internals::_lambda(f, &F::operator()))
	{
		return Internals::_lambda(f, &F::operator());
	}
}

//---------------------------------------------------------------------------
#endif
