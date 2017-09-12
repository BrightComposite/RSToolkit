//---------------------------------------------------------------------------

#pragma once

#ifndef ALIGNED_H
#define ALIGNED_H

//---------------------------------------------------------------------------

#include "memory.h"
#include <core/addition/wrapper.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<class T>
	class aligned
	{
		T * _ptr;
		
		template<class ... A>
		static forceinline T * create(A && ... args) {
			return new(aligned_alloc(alignof(T), sizeof(T))) T(forward<A>(args)...);
		}
		
		static forceinline void destroy(T * ptr) {
			ptr->~T();
			aligned_free(ptr);
		}
	
	public:
		member_cast(*_ptr, T);
		
		template<class U = T, useif<can_construct<U>::value>>
		forceinline aligned() : _ptr(create()) {}
		
		forceinline aligned(const aligned & s) : _ptr(create(s)) {}
		
		forceinline aligned(aligned && s) : _ptr(create(forward<aligned>(s))) {}
		
		template<class ... A, useif<(sizeof...(A) > 0), asd::not_same_types<aligned, asd::types<A...>>::value, can_construct<T, A...>::value>>
		forceinline aligned(A && ... args) : _ptr(create(forward<A>(args)...)) {}
		
		forceinline ~aligned() {
			if(_ptr) {
				destroy(_ptr);
			}
		}
		
		forceinline aligned & operator =(const aligned & s) {
			*_ptr = s;
			return *this;
		}
		
		forceinline aligned & operator =(aligned && s) {
			destroy(_ptr);
			_ptr = s._ptr;
			s._ptr = nullptr;
			
			return *this;
		}
		
		forceinline aligned & operator =(const T & value) {
			*_ptr = value;
			return *this;
		}
		
		template<class U, typename = decltype(declval<T>() == declval<U>())>
		forceinline bool operator ==(const U & value) const {
			return *_ptr == value;
		}
		
		template<class U, typename = decltype(declval<T>() != declval<U>())>
		forceinline bool operator !=(const U & value) const {
			return *_ptr != value;
		}
		
		forceinline T * operator ->() {
			return _ptr;
		}
		
		forceinline const T * operator ->() const {
			return _ptr;
		}
		
		forceinline T & operator *() {
			return *_ptr;
		}
		
		forceinline const T & operator *() const {
			return *_ptr;
		}
		
		template<class U, class = decltype(declval<T &>() += declval<U>())>
		forceinline aligned & operator +=(const aligned<U> & x) {
			*_ptr += *x;
			return *this;
		}
		
		template<class U, class = decltype(declval<T &>() += declval<U>())>
		forceinline aligned & operator +=(const U & x) {
			*_ptr += x;
			return *this;
		}
		
		template<class U, class = decltype(declval<T &>() -= declval<U>())>
		forceinline aligned & operator -=(const aligned<U> & x) {
			*_ptr -= *x;
			return *this;
		}
		
		template<class U, class = decltype(declval<T &>() -= declval<U>())>
		forceinline aligned & operator -=(const U & x) {
			*_ptr -= x;
			return *this;
		}
		
		template<class U, class = decltype(declval<T &>() *= declval<U>())>
		forceinline aligned & operator *=(const aligned<U> & x) {
			*_ptr *= *x;
			return *this;
		}
		
		template<class U, class = decltype(declval<T &>() *= declval<U>())>
		forceinline aligned & operator *=(const U & x) {
			*_ptr *= x;
			return *this;
		}
		
		template<class U, class = decltype(declval<T &>() /= declval<U>())>
		forceinline aligned & operator /=(const aligned<U> & x) {
			*_ptr /= *x;
			return *this;
		}
		
		template<class U, class = decltype(declval<T &>() /= declval<U>())>
		forceinline aligned & operator /=(const U & x) {
			*_ptr /= x;
			return *this;
		}
	};
	
	template<class T, size_t N>
	using aligned_array = aligned<asd::array<T, N>>;
	
	template<class A, class B>
	forceinline auto operator +(const aligned<A> & a, const aligned<B> & b) -> decltype(declval<A>() + declval<B>()) {
		return *a + *b;
	}
	
	template<class A, class B>
	forceinline auto operator +(const aligned<A> & a, const B & b) -> decltype(declval<A>() + declval<B>()) {
		return *a + b;
	}
	
	template<class A, class B>
	forceinline auto operator +(const A & a, const aligned<B> & b) -> decltype(declval<A>() + declval<B>()) {
		return a + *b;
	}
	
	template<class A, class B>
	forceinline auto operator -(const aligned<A> & a, const aligned<B> & b) -> decltype(declval<A>() - declval<B>()) {
		return *a - *b;
	}
	
	template<class A, class B>
	forceinline auto operator -(const aligned<A> & a, const B & b) -> decltype(declval<A>() - declval<B>()) {
		return *a - b;
	}
	
	template<class A, class B>
	forceinline auto operator -(const A & a, const aligned<B> & b) -> decltype(declval<A>() - declval<B>()) {
		return a - *b;
	}
	
	template<class A, class B>
	forceinline auto operator *(const aligned<A> & a, const aligned<B> & b) -> decltype(declval<A>() * declval<B>()) {
		return *a * *b;
	}
	
	template<class A, class B>
	forceinline auto operator *(const aligned<A> & a, const B & b) -> decltype(declval<A>() * declval<B>()) {
		return *a * b;
	}
	
	template<class A, class B>
	forceinline auto operator *(const A & a, const aligned<B> & b) -> decltype(declval<A>() * declval<B>()) {
		return a * *b;
	}
	
	template<class A, class B>
	forceinline auto operator /(const aligned<A> & a, const aligned<B> & b) -> decltype(declval<A>() / declval<B>()) {
		return *a / *b;
	}
	
	template<class A, class B>
	forceinline auto operator /(const aligned<A> & a, const B & b) -> decltype(declval<A>() / declval<B>()) {
		return *a / b;
	}
	
	template<class A, class B>
	forceinline auto operator /(const A & a, const aligned<B> & b) -> decltype(declval<A>() / declval<B>()) {
		return a / *b;
	}
}

//---------------------------------------------------------------------------
#endif
