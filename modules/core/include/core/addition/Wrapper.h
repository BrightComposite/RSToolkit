//---------------------------------------------------------------------------

#pragma once

#ifndef IMITATOR_H
#define IMITATOR_H

//---------------------------------------------------------------------------

#include <core/function/Function.h>
#include <core/Class.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class T, class Owner>
	class Wrapper;

	template<class T, class Owner = Empty>
	class Wrapper
	{
		template<class, class>
		friend class Wrapper;

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
	class Wrapper<T *, Owner>
	{
		template<class, class>
		friend class Wrapper;

		inline auto pointer() const
		{
			return static_cast<const Owner *>(this)->pointer();
		}

	public:
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
	class Wrapper<T, Empty> : public Wrapper<T, Wrapper<T, Empty>>
	{
		template<class, class>
		friend class Wrapper;

		typedef Wrapper<T, Wrapper<T, Empty>> Base;

	public:
		Wrapper() : Base(), _inner() {}
		Wrapper(const T & inner) : Base(), _inner(inner) {}
		Wrapper(T && inner) : Base(), _inner(forward<T>(inner)) {}
		template<class U, class V>
		Wrapper(const Wrapper<U, V> & im) : Base(), _inner(im._inner) {}
		template<class U, class V>
		Wrapper(Wrapper<U, V> && im) : Base(), _inner(move(im._inner)) {}

		template<class U, class V>
		inline Wrapper & operator = (const Wrapper<U, V> & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		static inline Wrapper create(A && ... args)
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
	class Wrapper<T *, Empty> : public Wrapper<T *, Wrapper<T *, Empty>>
	{
		template<class, class>
		friend class Wrapper;

	public:
		Wrapper() : _inner() {}
		Wrapper(T * inner) : _inner(inner) {}
		Wrapper(const Wrapper & im) : _inner(im._inner) {}
		template<class U, class V>
		Wrapper(const Wrapper<U *, V> & im) : _inner(im._inner) {}

		~Wrapper()
		{
			if(_inner) delete _inner;
		}

		inline Wrapper & operator = (const Wrapper & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class U, class V>
		inline Wrapper & operator = (const Wrapper<U *, V> & im)
		{
			_inner = im._inner;
			return *this;
		}
		
		template<class ... A, useif<can_construct<T, A...>::value>>
		static inline Wrapper create(A && ... args)
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
	class Wrapper<const T, Owner>
	{
		template<class, class>
		friend class Wrapper;

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
	class Wrapper<const T *, Owner>
	{
		template<class, class>
		friend class Wrapper;

		inline const T * pointer() const
		{
			return static_cast<const Owner *>(this)->pointer();
		}

	public:
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
	class Wrapper<const T, Empty> : public Wrapper<const T, Wrapper<const T, Empty>>
	{
		template<class, class>
		friend class Wrapper;

		typedef Wrapper<const T, Wrapper<const T, Empty>> Base;

	public:
		Wrapper() : Base(), _inner() {}
		Wrapper(const T & inner) : Base(), _inner(inner) {}
		Wrapper(T && inner) : Base(), _inner(forward<T>(inner)) {}
		template<class U, class V>
		Wrapper(const Wrapper<U, V> & im) : Base(), _inner(im._inner) {}
		template<class U, class V>
		Wrapper(Wrapper<U, V> && im) : Base(), _inner(move(im._inner)) {}

		template<class U, class V>
		inline Wrapper & operator = (const Wrapper<U, V> & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		static inline Wrapper create(A && ... args)
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
	class Wrapper<const T *, Empty> : public Wrapper<const T *, Wrapper<const T *, Empty>>
	{
		template<class, class>
		friend class Wrapper;

	public:
		Wrapper() : _inner() {}
		Wrapper(const T * inner) : _inner(inner) {}
		Wrapper(const Wrapper & im) : _inner(im._inner) {}
		template<class U, class V>
		Wrapper(const Wrapper<U *, V> & im) : _inner(im._inner) {}

		~Wrapper()
		{ 
			if(_inner) delete _inner;
		}

		inline Wrapper & operator = (const Wrapper & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class U, class V>
		inline Wrapper & operator = (const Wrapper<U*, V> & im)
		{
			_inner = im._inner;
			return *this;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		static inline Wrapper create(A && ... args)
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
