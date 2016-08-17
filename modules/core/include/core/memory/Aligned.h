//---------------------------------------------------------------------------

#pragma once

#ifndef ALIGNED_H
#define ALIGNED_H

//---------------------------------------------------------------------------

#include "Memory.h"
#include <core/addition/Wrapper.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class T>
	class Aligned
	{
		using Base = Wrapper<T, Aligned<T>>;

		T * _ptr;

		template<class ... A>
		static T * create(A &&... args)
		{
			return new (_aligned_malloc(sizeof(T), alignof(T))) T(forward<A>(args)...);
		}

		static void destroy(T * ptr)
		{
			ptr->~T();
			_aligned_free(ptr);
		}

	public:
		member_cast(*_ptr, T);

		template<class U = T, useif<can_construct<U>::value>>
		Aligned() : _ptr(create()) {}

		Aligned(const Aligned & s) : _ptr(create(s)) {}
		Aligned(Aligned && s) : _ptr(create(forward<Aligned>(s))) {}

		template<class ... A, useif<(sizeof...(A) > 0), not_same_types<Aligned, Types<A...>>::value, can_construct<T, A...>::value>>
		Aligned(A &&... args) : _ptr(create(forward<A>(args)...)) {}

		~Aligned()
		{
			if(_ptr)
				destroy(_ptr);
		}

		Aligned & operator = (const Aligned & s)
		{
			*_ptr = s;
			return *this;
		}

		Aligned & operator = (Aligned && s)
		{
			destroy(_ptr);
			_ptr = s._ptr;
			s._ptr = nullptr;

			return *this;
		}

		Aligned & operator = (const T & value)
		{
			*_ptr = value;
			return *this;
		}

		template<class U, typename = decltype(declval<T>() == declval<U>())>
		bool operator == (const U & value) const
		{
			return *_ptr == value;
		}

		template<class U, typename = decltype(declval<T>() != declval<U>())>
		bool operator != (const U & value) const
		{
			return *_ptr != value;
		}

		T * operator -> ()
		{
			return _ptr;
		}

		const T * operator -> () const
		{
			return _ptr;
		}

		T & operator * ()
		{
			return *_ptr;
		}

		const T & operator * () const
		{
			return *_ptr;
		}
	};

	template<class T, size_t N>
	using AlignedArray = Aligned<array<T, N>>;
}

//---------------------------------------------------------------------------
#endif
