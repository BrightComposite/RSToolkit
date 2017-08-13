//---------------------------------------------------------------------------

#pragma once

#ifndef FUNCTION_H
#define FUNCTION_H

//---------------------------------------------------------------------------

#include <functional>
#include <vector>

#include <meta/Types.h>
#include <meta/UseIf.h>

//---------------------------------------------------------------------------

namespace asd
{
	using std::function;

#if defined(__GNUC__)
#define invoke __invoke
#elif defined(_MSC_VER)
#define __thiscall ____thiscall
#endif // __GNUC__
	
	typedef function<void()> Action;
	
	class Anyway : public Action
	{
	public:
		~Anyway() {
			for(auto & f : _functions) {
				f();
			}
		}
		
		template<class F, class ... A, typename = decltype(std::invoke(declval<F>(), declval<A>()...))>
		void add(F f, A && ... args) {
			_functions.emplace_back(std::bind(f, forward<A>(args)...));
		}
	
	protected:
		std::vector<function<void()>> _functions;
	};
	
	template<typename R, typename ... A>
	using func_t = R (*)(A...);
	
	template<class T>
	struct is_std_function : false_type {};
	
	template<class T>
	struct is_std_function<function<T>> : true_type {};
	
	template<typename T, typename M>
	struct method_wrapper
	{
		typedef empty MethodType;
	};
	
	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<const T, R(C::*)(A...)>
	{
		typedef empty MethodType;
	};
	
	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<const T *, R(C::*)(A...)>
	{
		typedef empty MethodType;
	};
	
	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T, R(C::*)(A...)>
	{
		typedef R(__thiscall C::*MethodType)(A...);
		
		R operator ()(A ... args) {
			return (object.*method)(args...);
		}
		
		T object;
		MethodType method;
	};
	
	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T, R(C::*)(A...) const>
	{
		typedef R(__thiscall C::*MethodType)(A...) const;
		
		R operator ()(A ... args) const {
			return (object.*method)(forward<A>(args)...);
		}
		
		T object;
		MethodType method;
	};
	
	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T &, R(C::*)(A...)>
	{
		typedef R(__thiscall C::*MethodType)(A...);
		
		R operator ()(A ... args) {
			return (object.*method)(args...);
		}
		
		T & object;
		MethodType method;
	};
	
	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T &, R(C::*)(A...) const>
	{
		typedef R(__thiscall C::*MethodType)(A...) const;
		
		R operator ()(A ... args) const {
			return (object.*method)(forward<A>(args)...);
		}
		
		T & object;
		MethodType method;
	};
	
	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T *, R(C::*)(A...)>
	{
		typedef R(__thiscall C::*MethodType)(A...);
		
		R operator ()(A ... args) {
			return (object->*method)(args...);
		}
		
		T * object;
		MethodType method;
	};
	
	template<typename T, typename R, typename C, typename ... A>
	struct method_wrapper<T *, R(C::*)(A...) const>
	{
		typedef R(__thiscall C::*MethodType)(A...) const;
		
		R operator ()(A ... args) const {
			return (object->*method)(forward<A>(args)...);
		}
		
		T * object;
		MethodType method;
	};
	
	template<class T, class M>
	struct is_method : not_same_type<typename method_wrapper<T, M>::MethodType, empty> {};
	
	template<class T, class M, useif<!std::is_pointer<T>::value, is_method<T, M>::value>>
	forceinline method_wrapper<T &, M> wrap_method(T & object, M method) {
		return {object, method};
	}
	
	template<class T, class M, useif<!std::is_pointer<T>::value, is_method<T, M>::value>>
	forceinline method_wrapper<T, M> wrap_method(T && object, M method) {
		return {forward<T>(object), method};
	}
	
	template<class T, class M, useif<is_method<T *, M>::value>>
	forceinline method_wrapper<T *, M> wrap_method(T * object, M method) {
		return {object, method};
	}
	
	namespace internals
	{
		template<typename R, typename ... A>
		forceinline function<R(A...)> make_function(R(* f)(A...)) {
			return f;
		}
	}
	
	template<typename F, selectif(0)<is_function<F>::value>>
	
	forceinline auto make_function(F f) {
		return internals::make_function(f);
	}
	
	template<typename F, typename C, typename R, typename ... A, skipif<is_pointer<F>::value>>
	forceinline function<R(A...)> make_function(F & f, R(C::*mf)(A...)) {
		return wrap_method(f, mf);
	}
	
	template<typename F, typename C, typename R, typename ... A, skipif<is_pointer<F>::value>>
	forceinline function<R(A...)> make_function(F & f, R(C::*mf)(A...) const) {
		return wrap_method(f, mf);
	}
	
	template<typename F, typename C, typename R, typename ... A, skipif<is_pointer<F>::value>>
	forceinline function<R(A...)> make_function(F && f, R(C::*mf)(A...)) {
		return wrap_method(forward<F>(f), mf);
	}
	
	template<typename F, typename C, typename R, typename ... A, skipif<is_pointer<F>::value>>
	forceinline function<R(A...)> make_function(F && f, R(C::*mf)(A...) const) {
		return wrap_method(forward<F>(f), mf);
	}
	
	template<typename F, typename C, typename R, typename ... A>
	forceinline function<R(A...)> make_function(F * f, R(C::*mf)(A...)) {
		return wrap_method(f, mf);
	}
	
	template<typename F, typename C, typename R, typename ... A>
	forceinline function<R(A...)> make_function(F * f, R(C::*mf)(A...) const) {
		return wrap_method(f, mf);
	}
	
	template<typename F, selectif(1)<has_caller<F>::value>>
	
	forceinline auto make_function(F & f) {
		return make_function(f, &F::operator ());
	}
	
	template<typename F, selectif(1)<has_caller<F>::value>>
	
	forceinline auto make_function(F && f) {
		return make_function(forward<F>(f), &F::operator ());
	}
	
	template<typename F, selectif(1)<has_caller<F>::value>>
	
	forceinline auto make_function(F * f) {
		return make_function(f, &F::operator ());
	}
	
	template<class T, bool = is_pointerable<T>::value>
	struct member_owner
	{
		using type = std::decay_t<T>;
	};
	
	template<class T>
	struct member_owner<T, true>
	{
		using type = std::decay_t<typename std::pointer_traits<T>::element_type>;
	};
	
	template<class T>
	using member_owner_t = typename member_owner<T>::type;
	
	#define make_method(obj, method) wrap_method(obj, &member_owner_t<decltype(obj)>::method)
}

//---------------------------------------------------------------------------
#endif
