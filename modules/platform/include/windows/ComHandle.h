//---------------------------------------------------------------------------

#ifndef COM_HANDLE_H
#define COM_HANDLE_H

//---------------------------------------------------------------------------

#include <meta/Macro.h>

//---------------------------------------------------------------------------

namespace Rapture
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

		mutable T * _object;

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

		inline ComHandle & operator = (const T * & obj)
		{
			release();
			_object = obj;
			keep();

			return *this;
		}

		template<class U, class ... A>
		inline ComHandle & operator = (const ComHandle<U, A...> & com)
		{
			release();
			com.queryInterface(*this);

			return *this;
		}

		inline ComHandle & operator = (nullptr_t)
		{
			release();
			return *this;
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

		template<class U, class ... A>
		inline ComHandle(const ComHandle<U, A...> & com) : _object(nullptr)
		{
			com.queryInterface(*this);
		}

		inline ComHandle(T * obj) : _object(obj)
		{
			keep();
		}

		virtual ~ComHandle()
		{
			release();
		}

		bool operator == (const ComHandle & com) const
		{
			return (com._object == nullptr) ? (_object == nullptr) : com._object == _object;
		}

		bool operator == (const T * object) const
		{
			return (object == nullptr) ? (_object == nullptr) : object == _object;
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

		inline T ** operator & () const
		{
			return &_object;
		}

		inline void ** pointer() const
		{
			return reinterpret_cast<void **>(&_object);
		}

		template<class U, class ... A>
		void queryInterface(ComHandle<U, A...> & com) const
		{
			com_assert(
				_object->QueryInterface(__uuidof (U), com.pointer())
				);
		}
	};

	template<class T>
	class ComHandle<T> : public ComHandle<T, Empty>
	{
		typedef ComHandle<T, Empty> Base;

	public:
		using Base::ComHandle;

		inline ComHandle() : Base() {}
		inline ComHandle(const ComHandle & com) : Base(com) {}
		inline ComHandle(ComHandle && com) : Base(forward<ComHandle>(com)) {}
		template<class U, class ... A>
		inline ComHandle(const ComHandle<U, A...> & com) : Base(com) {}

		virtual ~ComHandle() {}

		inline ComHandle & operator = (const ComHandle & com)
		{
			Base::operator = (com);
			return *this;
		}

		inline ComHandle & operator = (ComHandle && com)
		{
			Base::operator = (forward<ComHandle>(com));
			return *this;
		}

		template<class U, class ... A>
		inline ComHandle & operator = (const ComHandle<U, A...> & com)
		{
			Base::operator = (com);
			return *this;
		}

		inline ComHandle & operator = (const T * & obj)
		{
			Base::operator = (obj);
			return *this;
		}

		inline ComHandle & operator = (nullptr_t)
		{
			Base::operator = (nullptr);
			return *this;
		}
	};
}

//---------------------------------------------------------------------------
#endif
