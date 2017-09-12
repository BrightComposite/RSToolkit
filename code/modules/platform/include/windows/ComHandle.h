//---------------------------------------------------------------------------

#pragma once

#ifndef COM_HANDLE_H
#define COM_HANDLE_H

//---------------------------------------------------------------------------

#include <meta/macro.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<class T, class ... A>
	class ComHandle {};

	template<class T, class Owner>
	class ComHandle<T, Owner>
	{
		friend Owner;

		template<class, class ...>
		friend class ComHandle;

		static_assert(is_base_of<IUnknown, T>::value, "T class must implement IUnknown!");

		T * _object;

		void keep()
		{
			if(_object != nullptr)
				_object->AddRef();
		}

		void release()
		{
			if(_object != nullptr)
			{
				_object->Release();
				_object = nullptr;
			}
		}

		inline ComHandle & operator = (const ComHandle & com)
		{
			release();
			_object = com._object;
			keep();

			return *this;
		}

		inline ComHandle & operator = (ComHandle && com)
		{
			release();
			_object = com._object;
			com._object = nullptr;

			return *this;
		}

		inline ComHandle & operator = (const T * obj)
		{
			release();
			_object = obj;
			keep();

			return *this;
		}

		inline ComHandle & operator = (nullptr_t)
		{
			release();
			return *this;
		}

		inline ComHandle(T * obj) : _object(obj)
		{
			keep();
		}

	public:
		inline ComHandle() : _object(nullptr) {}

		inline ComHandle(const ComHandle & com) : _object(com._object)
		{
			keep();
		}

		inline ComHandle(ComHandle && com) : _object(com._object)
		{
			com._object = nullptr;
		}

		virtual ~ComHandle()
		{
			release();
		}

		bool operator == (const ComHandle & com) const
		{
			return com._object == _object;
		}

		bool operator == (const T * object) const
		{
			return object == _object;
		}

		bool operator == (nullptr_t) const
		{
			return _object == nullptr;
		}

		operator bool () const
		{
			return _object != nullptr;
		}

		template<class U>
		bool operator != (const U & x) const
		{
			return !operator == (x);
		}

		inline T * operator -> () const
		{
			return _object;
		}

		inline operator T * () const
		{
			return _object;
		}

		inline T ** operator & ()
		{
			return &_object;
		}

		inline T * const * operator & () const
		{
			return &_object;
		}

		inline void ** pointer()
		{
			return reinterpret_cast<void **>(&_object);
		}

		inline void * const * pointer() const
		{
			return reinterpret_cast<void **>(&_object);
		}

		template<class U, class ... A>
		void queryInterface(ComHandle<U, A...> & com) const
		{
			com.release();
			com_assert(_object->QueryInterface(__uuidof (U), com.pointer()));
		}
	};

	template<class T>
	class ComHandle<T> : public ComHandle<T, Empty>
	{
		typedef ComHandle<T, Empty> base_type;

	public:
		using base_type::ComHandle;

		inline ComHandle() : base_type() {}
		inline ComHandle(const ComHandle & com) : base_type(com) {}
		inline ComHandle(ComHandle && com) : base_type(forward<ComHandle>(com)) {}

		virtual ~ComHandle() {}

		inline ComHandle & operator = (const ComHandle & com)
		{
			base_type::operator = (com);
			return *this;
		}

		inline ComHandle & operator = (ComHandle && com)
		{
			base_type::operator = (forward<ComHandle>(com));
			return *this;
		}

		inline ComHandle & operator = (const T * obj)
		{
			base_type::operator = (obj);
			return *this;
		}

		inline ComHandle & operator = (nullptr_t)
		{
			base_type::operator = (nullptr);
			return *this;
		}
	};
}

//---------------------------------------------------------------------------
#endif
