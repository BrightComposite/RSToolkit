//---------------------------------------------------------------------------

#pragma once

#ifndef STORAGE_H
#define STORAGE_H

//---------------------------------------------------------------------------

#include "Memory.h"
#include <core/addition/Wrapper.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class T>
	class Storage
	{
		using Base = Wrapper<T, Storage<T>>;

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
		Storage() : _ptr(create()) {}

		Storage(const Storage & s) : _ptr(create(s)) {}
		Storage(Storage && s) : _ptr(create(forward<Storage>(s))) {}

		template<class ... A, useif<(sizeof...(A) > 0), not_same_types<Storage, Types<A...>>::value, can_construct<T, A...>::value>>
		Storage(A &&... args) : _ptr(create(forward<A>(args)...)) {}

		~Storage()
		{
			if(_ptr)
				destroy(_ptr);
		}

		Storage & operator = (const Storage & s)
		{
			*_ptr = s;
			return *this;
		}

		Storage & operator = (Storage && s)
		{
			destroy(_ptr);
			_ptr = s._ptr;
			s._ptr = nullptr;

			return *this;
		}

		Storage & operator = (const T & value)
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
}

//---------------------------------------------------------------------------
#endif
