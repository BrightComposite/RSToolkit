//---------------------------------------------------------------------------

#pragma once

#ifndef IMITATOR_H
#define IMITATOR_H

//---------------------------------------------------------------------------

#include <function/method.h>
#include <core/meta_class.h>
#include <core/Hash.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<class T, class Owner>
	class wrapper;

	template<class T, class Owner = empty>
	class wrapper
	{
		template<class, class>
		friend class wrapper;

		inline T & in_() const
		{
			return static_cast<const Owner *>(this)->in_();
		}

	public:
		inline T & inner() const
		{
			return in_();
		}

		inline operator T & () const
		{
			return in_();
		}
	};

	template<class T, class Owner>
	class wrapper<T *, Owner>
	{
		template<class, class>
		friend class wrapper;

		inline T * pointer() const
		{
			return static_cast<const Owner *>(this)->pointer();
		}

	public:
		bool isNull() const {
			return pointer() == nullptr;
		}
		
		template<class U, class O>
		bool operator ==(const wrapper<U, O> & w) const {
			return pointer() == w.pointer();
		}
		
		bool operator == (T * ptr) const {
			return pointer() == ptr;
		}
		
		bool operator == (nullptr_t) const {
			return pointer() == nullptr;
		}
		
		template<class U>
		bool operator != (const U & x) const {
			return !operator == (x);
		}
		
		template<class U, class O>
		bool operator > (const wrapper<U, O> & w) const {
			return pointer() > w.pointer();
		}
		
		template<class U, class O>
		bool operator < (const wrapper<U, O> & w) const {
			return pointer() < w.pointer();
		}
		
		template<class U, class O>
		bool operator >= (const wrapper<U, O> & w) const {
			return pointer() >= w.pointer();
		}
		
		template<class U, class O>
		bool operator <= (const wrapper<U, O> & w) const {
			return pointer() <= w.pointer();
		}
		
		size_t hash() const {
			return ptr_hash<T>(pointer());
		}
		
		inline T * inner() const
		{
			return pointer();
		}
		
		inline T * operator -> () const
		{
			return pointer();
		}

		template<typename R, typename ... A>
		inline auto operator ->* (R(__thiscall T::*method)(A...)) const
		{
			return wrap_method(pointer(), method);
		}

		template<typename R, typename ... A>
		inline auto operator ->* (R(__thiscall T::*method)(A...) const) const
		{
			return wrap_method(pointer(), method);
		}

		template<typename V, useif<std::is_member_pointer<V>::value>>
		inline V & operator ->* (V T::*member) const
		{
			return pointer()->*member;
		}

		inline operator T * () const
		{
			return pointer();
		}

		inline T & operator * () const
		{
			return *pointer();
		}
	};

	template<class T>
	class wrapper<T, empty> : public wrapper<T, wrapper<T, empty>>
	{
		template<class, class>
		friend class wrapper;

		typedef wrapper<T, wrapper<T, empty>> base_type;

	public:
		wrapper() : base_type(), _inner() {}
		wrapper(const T & inner) : base_type(), _inner(inner) {}
		wrapper(T && inner) : base_type(), _inner(forward<T>(inner)) {}
		template<class U, class V>
		wrapper(const wrapper<U, V> & im) : base_type(), _inner(im._inner) {}
		template<class U, class V>
		wrapper(wrapper<U, V> && im) : base_type(), _inner(move(im._inner)) {}

		template<class U, class V>
		inline wrapper & operator = (const wrapper<U, V> & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		static inline wrapper create(A && ... args)
		{
			return T(forward<A>(args)...);
		}

	protected:
		mutable T _inner;

		inline T & in_() const
		{
			return _inner;
		}
	};

	template<class T>
	class wrapper<T *, empty> : public wrapper<T *, wrapper<T *, empty>>
	{
		template<class, class>
		friend class wrapper;

	public:
		wrapper() : _inner() {}
		wrapper(T * inner) : _inner(inner) {}
		wrapper(const wrapper & im) : _inner(im._inner) {}
		template<class U, class V>
		wrapper(const wrapper<U *, V> & im) : _inner(im._inner) {}

		~wrapper()
		{
			if(_inner) delete _inner;
		}

		inline wrapper & operator = (const wrapper & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class U, class V>
		inline wrapper & operator = (const wrapper<U *, V> & im)
		{
			_inner = im._inner;
			return *this;
		}
		
		template<class ... A, useif<can_construct<T, A...>::value>>
		static inline wrapper create(A && ... args)
		{
			return new T(forward<A>(args)...);
		}

	protected:
		mutable T * _inner;

		inline T * pointer() const
		{
			return _inner;
		}
	};

	template<class T, class Owner>
	class wrapper<const T, Owner>
	{
		template<class, class>
		friend class wrapper;

		inline const T & in_() const
		{
			return static_cast<Owner *>(this)->in_();
		}

	public:
		inline const T & inner() const
		{
			return in_();
		}

		inline operator const T & () const
		{
			return in_();
		}
	};

	template<class T, class Owner>
	class wrapper<const T *, Owner>
	{
		template<class, class>
		friend class wrapper;

		inline const T * pointer() const
		{
			return static_cast<const Owner *>(this)->pointer();
		}

	public:
		bool isNull() const {
			return pointer() == nullptr;
		}
		
		template<class U, class O>
		bool operator ==(const wrapper<U, O> & w) const {
			return pointer() == w.pointer();
		}
		
		bool operator == (T * ptr) const {
			return pointer() == ptr;
		}
		
		bool operator == (nullptr_t) const {
			return pointer() == nullptr;
		}
		
		template<class U>
		bool operator != (const U & x) const {
			return !operator == (x);
		}
		
		template<class U, class O>
		bool operator > (const wrapper<U, O> & w) const {
			return pointer() > w.pointer();
		}
		
		template<class U, class O>
		bool operator < (const wrapper<U, O> & w) const {
			return pointer() < w.pointer();
		}
		
		template<class U, class O>
		bool operator >= (const wrapper<U, O> & w) const {
			return pointer() >= w.pointer();
		}
		
		template<class U, class O>
		bool operator <= (const wrapper<U, O> & w) const {
			return pointer() <= w.pointer();
		}
		
		size_t hash() const {
			return ptr_hash<T>(pointer());
		}
		
		inline const T * inner() const
		{
			return pointer();
		}

		inline const T * operator -> () const
		{
			return pointer();
		}

		template<typename R, typename ... A>
		inline auto operator ->* (R (T::*method)(A...) const) const
		{
			return wrap_method(pointer(), method);
		}

		template<typename V, useif<std::is_member_pointer<V>::value>>
		inline const V & operator ->* (V T::*member) const
		{
			return pointer()->*member;
		}

		inline operator const T * () const
		{
			return pointer();
		}

		inline const T & operator * () const
		{
			return *pointer();
		}
	};

	template<class T>
	class wrapper<const T, empty> : public wrapper<const T, wrapper<const T, empty>>
	{
		template<class, class>
		friend class wrapper;

		typedef wrapper<const T, wrapper<const T, empty>> base_type;

	public:
		wrapper() : base_type(), _inner() {}
		wrapper(const T & inner) : base_type(), _inner(inner) {}
		wrapper(T && inner) : base_type(), _inner(forward<T>(inner)) {}
		template<class U, class V>
		wrapper(const wrapper<U, V> & im) : base_type(), _inner(im._inner) {}
		template<class U, class V>
		wrapper(wrapper<U, V> && im) : base_type(), _inner(move(im._inner)) {}

		template<class U, class V>
		inline wrapper & operator = (const wrapper<U, V> & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		static inline wrapper create(A && ... args)
		{
			return T(forward<A>(args)...);
		}

	protected:
		const T _inner;

		inline const T & in_() const
		{
			return _inner;
		}
	};

	template<class T>
	class wrapper<const T *, empty> : public wrapper<const T *, wrapper<const T *, empty>>
	{
		template<class, class>
		friend class wrapper;

	public:
		wrapper() : _inner() {}
		wrapper(const T * inner) : _inner(inner) {}
		wrapper(const wrapper & im) : _inner(im._inner) {}
		template<class U, class V>
		wrapper(const wrapper<U *, V> & im) : _inner(im._inner) {}

		~wrapper()
		{ 
			if(_inner) delete _inner;
		}

		inline wrapper & operator = (const wrapper & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class U, class V>
		inline wrapper & operator = (const wrapper<U*, V> & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		static inline wrapper create(A && ... args)
		{
			return new T(forward<A>(args)...);
		}

	protected:
		const T * _inner;

		inline const T * inptr_() const
		{
			return _inner;
		}
	};
}

//---------------------------------------------------------------------------
#endif
