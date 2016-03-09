//---------------------------------------------------------------------------

#ifndef ACTION_H
#define ACTION_H

//---------------------------------------------------------------------------

#include <functional>
#include <core/meta/Require.h>
#include <core/meta/Types.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::function;

	typedef function<void()> Action;

	class ImmediateAction : public Action
	{
	public:
		template<class F,
			require(
				is_same<void, decltype(std::invoke(declval<F>()))>::value
				)>
		ImmediateAction(F f) : Action(f) { operator()(); }

		template<class F, class ... A,
			require(
				is_same<void, std::result_of<F(A &&...)>::type>::value &&
				sizeof ... (A) > 0
				)>
		ImmediateAction(F f, A &&... args) : Action(std::bind(f, forward<A>(args)...)) { operator()(); }
		~ImmediateAction() {}
	};

	class FinalAction : public Action
	{
	public:
		template<class F,
			require(
				is_same<void, decltype(std::invoke(declval<F>()))>::value
				)>
		FinalAction(F f) : Action(f) {}

		template<class F, class ... A,
			require(
				is_same<void, std::result_of<F(A &&...)>::type>::value &&
				sizeof ... (A) > 0
				)>
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
	struct method_wrapper<T, R(__thiscall C::*)(A &&...)>
	{
		typedef R(__thiscall C::*MethodType)(A &&...);

		R operator()(A &&... args)
		{
			return (object.*method)(forward<A>(args)...);
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
	struct method_wrapper<T *, R (__thiscall C::*)(A &&...)>
	{
		typedef R (__thiscall C::*MethodType)(A &&...);

		R operator()(A &&... args)
		{
			return (object->*method)(forward<A>(args)...);
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
			return (object.*method)(args...);
		}

		T & object;
		MethodType method;
	};

	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T, R(__thiscall C::*)(A &&...) const>
	{
		typedef R(__thiscall C::*MethodType)(A &&...) const;

		R operator()(A &&... args)
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
			return (object->*method)(args...);
		}

		T * object;
		MethodType method;
	};

	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T *, R(__thiscall C::*)(A &&...) const>
	{
		typedef R(__thiscall C::*MethodType)(A &&...) const;

		R operator()(A &&... args)
		{
			return (object->*method)(forward<A>(args)...);
		}

		T * object;
		MethodType method;
	};

	template<class T, class M>
	struct is_method
	{
		static const bool value = !is_same<typename method_wrapper<T, M>::MethodType, Empty>::value;
	};

	template<class T, class M,
		require(
			!std::is_pointer<T>::value &&
			is_method<T, M>::value
			)>
	method_wrapper<T, M> wrap_method(T & object, M method)
	{
		return {object, method};
	}

	template<class T, class M,
		require(
			is_method<T *, M>::value
			)>
	method_wrapper<T *, M> wrap_method(T * object, M method)
	{
		return {object, method};
	}

	namespace Internals
	{
		template<typename F, typename R, typename ... A>
		auto _lambda(F & f, R(F::*mf) (A...) const) -> std::enable_if_t<std::is_convertible<F, func_t<R, A...>>::value, func_t<R, A...>>
		{
			return f;
		}

		template<typename F, typename R, typename ... A>
		auto _lambda(F & f, R(F::*mf) (A...)) -> std::enable_if_t<std::is_convertible<F, func_t<R, A...>>::value, func_t<R, A...>>
		{
			return f;
		}
	}

	/**
	 *	Works only for non-closure lambda
	 */
	template<typename F>
	auto lambda(F & f) -> decltype(Internals::_lambda(f, &F::operator()))
	{
		return Internals::_lambda(f, &F::operator());
	}
}

//---------------------------------------------------------------------------
#endif
