//---------------------------------------------------------------------------

#pragma once

#ifndef HANDLE_H
#define HANDLE_H

//---------------------------------------------------------------------------

#include <core/addition/Singleton.h>
#include <core/addition/Wrapper.h>
#include <core/memory/allocator/DefaultAllocator.h>
#include <core/Hash.h>
#include <container/Container.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class ... A>
	class Handle;

	template<class ... A>
	class UniqueHandle;

	//---------------------------------------------------------------------------

	template<class T>
	struct PointerDeleter
	{
		static void free(const T * ptr)
		{
			delete ptr;
		}

		void operator()(const T * ptr)
		{
			delete ptr;
		}
	};

	//---------------------------------------------------------------------------

//#define ATOMIC_REFERENCES

#ifdef ATOMIC_REFERENCES
	using ref_counter_t = atomic<size_t>;
#else
	using ref_counter_t = size_t;
#endif

    /**
     *  @brief
     *  The Shared class is used to store the reference counter which can be
	 *	accessed by the Handle class.
	 *
	 *	The Shared class by itself is only used by "shareable" objects.
	 *	A shareable object is an object of a class T which inherits the Shared
	 *	class. Such thing allows to create handles to sheareables without
	 *	wrapping them into SharedWrappers. Create sheareable classes when you
	 *	know that objects of these classes will be handled (you will almost
	 *	always want that).
     */
	struct Shared : protected DefaultAllocator
	{
		template<class ...> friend class Handle;
		template<class ...> friend class UniqueHandle;
		template<class>		friend struct PointerDeleter;

	protected:
		mutable ref_counter_t _refs = 1;
	};

	template<class T>
	class UniquePtr : public std::unique_ptr<T, PointerDeleter<T>>
	{
	public:
		using Base = std::unique_ptr<T, PointerDeleter<T>>;
		using Base::unique_ptr;

		UniquePtr & operator = (nullptr_t)
		{
			Base::operator = (nullptr);
			return (*this);
		}

		template<class U>
		UniquePtr & operator = (U && right)
		{
			Base::operator = (forward<U>(right));
			return (*this);
		}

		operator T * ()
		{
			return Base::get();
		}

		operator const T * () const
		{
			return Base::get();
		}
	};

	struct SharedEmpty : Empty, Shared {};

	template<class T>
	using is_shareable = is_base_of<Shared, T>;

//---------------------------------------------------------------------------

	/**
	 *  @brief
	 *	Template struct which provides casts for different handles.
	 */
	template<class T>
	struct SharedCast
	{
		static T * toObj(T * shared)
		{
			return shared;
		}

		template<class U>
		static T * cast(U * shared)
		{
			return static_cast<T *>(shared);
		}

		static T * toShared(T & x)
		{
			return &x;
		}
	};

//---------------------------------------------------------------------------

	template<class T>
	struct is_handle;

	template<class T, class H>
	struct is_handle_of;

	template<class T, class ... A>
	struct is_handle_init;

//---------------------------------------------------------------------------

	/**
	 *  @brief
	 *  The Handle class is the very important component in
	 *	memory-management of Rapture State Toolkit. It is used to fully
	 *	control life-time of objects.
	 *  It contains functions for allocating, deallocating, referencing and
	 *  releasing of objects of the type T.
	 *  The objects of this class store only pointer to the object.
	 *  They may be statically casted to object-pointer or shared-pointer.
	 *  Use operator *() to get object.
	 *  Use this class with pointers to objects (or shared objects, of
	 *	course).
	 *	The common version of the Handle class contains Owner template
	 *	argument. Assignment of objects of this class is denied, and only
	 *	Owner can do that.
	 */
	template<class ... A>
	class Handle {};

	/**
	 *  @brief
	 *  Disowned version of the Handle class
	 */
	template<class T>
	class Handle<T> : public Handle<T, Empty>
	{
		using Base = Handle<T, Empty>;

	public:
		Handle() : Base() {}

		Handle(const Handle & h) : Base(h) {}
		Handle(Handle && h) : Base(forward<Handle>(h)) {}

		Handle(T * shared) : Base(shared) {}
		Handle(nullptr_t) : Base(nullptr) {}

		template<class U, class ... A, useif<based_on<U, T>::value && !is_const<U>::value>>
		Handle(const Handle<U, A...> & h) : Base(h) {}
		template<class U, class ... A, useif<based_on<U, T>::value && !is_const<U>::value>>
		Handle(Handle<U, A...> && h) : Base(forward<Handle<U, A...>>(h)) {}

		template<class U = T, useif<can_construct<U>::value>>
		Handle(Empty) : Base(nothing) {}

		template<class ... A, selectif(0)<can_construct<T, A...>::value, (sizeof...(A) > 0)>>
		explicit Handle(A &&... args) : Base(forward<A>(args)...) {}

		template<class ... A, selectif(1)<is_abstract<T>::value, !is_handle_init<T, A...>::value>>
		explicit Handle(A &&... args) { static_assert(!is_abstract<T>::value, "Can't construct an abstract class"); }

		Handle & operator = (const Handle & h)
		{
			Base::operator = (h);
			return *this;
		}

		Handle & operator = (Handle && h)
		{
			Base::operator = (forward<Handle<T>>(h));
			return *this;
		}

		template<class U, class ... A, useif<based_on<U, T>::value && !is_const<U>::value>>
		Handle & operator = (const Handle<U, A...> & h)
		{
			Base::operator = (h);
			return *this;
		}

		template<class U, class ... A, useif<based_on<U, T>::value && !is_const<U>::value>>
		Handle & operator = (Handle<U, A...> && h)
		{
			Base::operator = (forward<Handle<U, A...>>(h));
			return *this;
		}

		Handle & operator = (T * shared)
		{
			Base::operator = (shared);
			return *this;
		}

		template<typename ... A, selectif(0)<can_construct<T, A...>::value>>
		Handle & init(A &&... args)
		{
			Base::init(forward<A>(args)...);
			return *this;
		}

		template<typename ... A, selectif(0)<can_construct<T, A...>::value>>
		static Handle create(A &&... args)
		{
			T * sh = new T(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A, selectif(1)<cant_construct<T, A...>::value, is_abstract<T>::value>>
		void init(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}

		template<typename ... A, selectif(1)<cant_construct<T, A...>::value, is_abstract<T>::value>>
		static void create(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}

		template<class U>
		static Handle cast(const Handle<U> & handle)
		{
			return static_cast<T *>(handle._shared);
		}
	};

	/**
	 *  @brief
	 *  Common owned version of the Handle class
	 */
	template<class T, class Owner>
	class Handle<T, Owner> : public Wrapper<T *, Handle<T, Owner>>, protected DefaultAllocator
    {
		template<class...>
		friend class Handle;

		friend struct Shared;
		friend Owner;

	protected:
		using Base = Wrapper<T *, Handle<T, Owner>>;

		friend Base;

		T * _shared;

		Handle(T * shared) : _shared(shared) { keep(); }

		template<class U = T, useif<can_construct<U>::value>>
		Handle(Empty) : _shared(new T()) {}

		template<class U, class ... O, useif<based_on<U, T>::value, !is_const<U>::value>, skipif<same_types<Owner, Types<O...>>::value>>
		Handle(const Handle<U, O...> & h) : _shared(static_cast<T *>(h._shared)) { keep(); }
		template<class U, class ... O, useif<based_on<U, T>::value, !is_const<U>::value>, skipif<same_types<Owner, Types<O...>>::value>>
		Handle(Handle<U, O...> && h) : _shared(static_cast<T *>(h._shared)) { h._shared = nullptr; }

		template<class ... A, selectif(0)<can_construct<T, A...>::value, (sizeof...(A) > 0)>>
		explicit Handle(A &&... args) : _shared(new T(forward<A>(args)...)) {}

		template<class ... A, selectif(1)<is_abstract<T>::value, !is_handle_init<T, A...>::value>>
		explicit Handle(A &&... args) { static_assert(!is_abstract<T>::value, "Can't construct an abstract class"); }

		void keep()
		{
			static_assert(based_on<T, Shared>::value, "T must inherit the Shared type");

			if(_shared != nullptr)
				++_shared->_refs;
		}

		void release()
		{
			if(_shared != nullptr)
			{
				--_shared->_refs;

				if(_shared->_refs <= 0)
					PointerDeleter<T>::free(_shared);
			}
		}

		template<class U, class ... O, useif<based_on<U, T>::value, !is_const<U>::value>, skipif<same_types<Owner, Types<O...>>::value>>
		Handle & operator = (const Handle<U, O...> & h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = static_cast<T *>(h._shared);
			keep();

			return *this;
		}

		template<class U, class ... O, useif<based_on<U, T>::value, !is_const<U>::value>, skipif<same_types<Owner, Types<O...>>::value>>
		Handle & operator = (Handle<U, O...> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = static_cast<T *>(h._shared);
			h._shared = nullptr;

			return *this;
		}

		Handle & operator = (T * shared)
		{
			release();
			_shared = shared;
			keep();

			return *this;
		}

		template<typename ... A, selectif(0)<can_construct<T, A...>::value>>
		Handle & init(A &&... args)
		{
			release();
			_shared = new T(forward<A>(args)...);

			return *this;
		}

		template<typename ... A, selectif(1)<cant_construct<T, A...>::value, is_abstract<T>::value>>
		void init(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}

		template<typename ... A, selectif(0)<can_construct<T, A...>::value>>
		static Handle create(A &&... args)
		{
			T * sh = new T(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A, selectif(1)<cant_construct<T, A...>::value, is_abstract<T>::value>>
		static void create(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}

	public:
		Handle() : _shared(nullptr) {}
		Handle(nullptr_t) : _shared(nullptr) {}

		Handle(const Handle & h) : _shared(h._shared) { keep(); }
		Handle(Handle && h) : _shared(h._shared) { h._shared = nullptr; }

		template<class U, useif<based_on<U, T>::value, not_same_type<U, T>::value, !is_const<U>::value>>
		Handle(const Handle<U, Owner> & h) : _shared(static_cast<T *>(h._shared)) { keep(); }
		template<class U, useif<based_on<U, T>::value, not_same_type<U, T>::value, !is_const<U>::value>>
		Handle(Handle<U, Owner> && h) : _shared(static_cast<T *>(h._shared)) { h._shared = nullptr; }

		~Handle() { release(); }

		bool isNull() const
		{
			return _shared == nullptr;
		}

		Handle & operator = (const Handle & h)
		{
			if(this == &h)
				return *this;

			release();
			_shared = h._shared;
			keep();

			return *this;
		}

		Handle & operator = (Handle && h)
		{
			if(this == &h)
				return *this;

			release();
			_shared = h._shared;
			h._shared = nullptr;

			return *this;
		}

		template<class U, useif<based_on<U, T>::value, not_same_type<U, T>::value, !is_const<U>::value>>
		Handle & operator = (const Handle<U, Owner> & h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = static_cast<T *>(h._shared);
			keep();

			return *this;
		}

		template<class U, useif<based_on<U, T>::value, not_same_type<U, T>::value, !is_const<U>::value>>
		Handle & operator = (Handle<U, Owner> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = static_cast<T *>(h._shared);
			h._shared = nullptr;

			return *this;
		}

		template<class U, class ... A>
		bool operator == (const Handle<U, A...> & h) const
		{
			return _shared == h._shared;
		}

		bool operator == (T * shared) const
		{
			return _shared == shared;
		}

		bool operator == (nullptr_t) const
		{
			return _shared == nullptr;
		}

		template<class U>
		bool operator != (const U & x) const
		{
			return !operator == (x);
		}

		template<class U>
		bool operator > (const Handle<U> & h) const
		{
			return _shared > h._shared;
		}

		template<class U>
		bool operator < (const Handle<U> & h) const
		{
			return _shared < h._shared;
		}

		T * pointer() const
		{
			return _shared;
		}

		operator T * () const
		{
			return _shared;
		}

		size_t refs() const
		{
			return _shared->_refs;
		}

		size_t hash() const
		{
			return ptr_hash<T>(_shared);
		}

		template<class U>
		static Handle cast(const Handle<U, Owner> & h)
		{
			return static_cast<T *>(h._shared);
		}
	};

	template<class T>
	class Handle<const T> : public Handle<const T, Empty>
	{
		using Base = Handle<const T, Empty>;

	public:
		Handle() : Base() {}

		Handle(const Handle & h) : Base(h) {}
		Handle(Handle && h) : Base(forward<Handle>(h)) {}

		Handle(const Handle<T> & h) : Base(h) {}
		Handle(Handle<T> && h) : Base(forward<Handle<T>>(h)) {}

		Handle(const T * shared) : Base(shared) {}
		Handle(nullptr_t) : Base(nullptr) {}

		template<class U, class ... A, useif<based_on<U, T>::value>>
		Handle(const Handle<U, A...> & h) : Base(h) {}
		template<class U, class ... A, useif<based_on<U, T>::value>>
		Handle(Handle<U, A...> && h) : Base(forward<Handle<U, A...>>(h)) {}

		template<class U = T, useif<can_construct<U>::value>>
		Handle(Empty) : Base(nothing) {}

		template<class ... A, selectif(0)<can_construct<T, A...>::value && (sizeof...(A) > 0)>>
		explicit Handle(A &&... args) : Base(forward<A>(args)...) {}
		template<class ... A, selectif(1)<is_abstract<T>::value, !is_handle_init<T, A...>::value>>
		explicit Handle(A &&... args) { static_assert(!is_abstract<T>::value, "Can't construct an abstract class"); }

		Handle & operator = (const Handle & h)
		{
			Base::operator = (h);
			return *this;
		}

		Handle & operator = (Handle && h)
		{
			Base::operator = (forward<Handle>(h));
			return *this;
		}

		template<class U, class ... A, useif<based_on<U, T>::value>>
		Handle & operator = (const Handle<U, A...> & h)
		{
			Base::operator = (h);
			return *this;
		}

		template<class U, class ... A, useif<based_on<U, T>::value>>
		Handle & operator = (Handle<U, A...> && h)
		{
			Base::operator = (forward<Handle<U, A...>>(h));
			return *this;
		}

		Handle & operator = (const T * shared)
		{
			Base::operator = (shared);
			return *this;
		}

		template <typename ... A, selectif(0)<can_construct<T, A...>::value>>
		Handle & init(A &&... args)
		{
			Base::init(forward<A>(args)...);
			return *this;
		}

		template<typename ... A, selectif(0)<can_construct<T, A...>::value>>
		static Handle create(A &&... args)
		{
			auto sh = new T(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A, selectif(1)<cant_construct<T, A...>::value, is_abstract<T>::value>>
		void init(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}

		template<typename ... A, selectif(1)<cant_construct<T, A...>::value, is_abstract<T>::value>>
		static void create(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}

		template<class U>
		static Handle cast(const Handle<const U> & handle)
		{
			return static_cast<const T *>(handle._shared);
		}
	};

	template<class T, class Owner>
	class Handle<const T, Owner> : public Wrapper<const T *, Handle<const T, Owner>>, protected DefaultAllocator
	{
		template<class...>
		friend class Handle;

		friend struct Shared;
		friend Owner;

	protected:
		using Base = Wrapper<const T *, Handle<const T, Owner>>;

		friend Base;

		const T * _shared;

		Handle(const T * shared) : _shared(shared) { keep(); }

		template<class U, class ... A, useif<based_on<U, T>::value>>
		Handle(const Handle<U, A...> & h) : _shared(static_cast<const T *>(h._shared)) { keep(); }
		template<class U, class ... A, useif<based_on<U, T>::value>>
		Handle(Handle<U, A...> && h) : _shared(static_cast<const T *>(h._shared)) { h._shared = nullptr; }

		template<class U = T, useif<can_construct<U>::value>>
		Handle(Empty) : _shared(new T()) {}

		template<class ... A, selectif(0)<can_construct<T, A...>::value && (sizeof...(A) > 0)>>
		explicit Handle(A && ... args) : _shared(new T(forward<A>(args)...)) {}
		template<class ... A, selectif(1)<is_abstract<T>::value, !is_handle_init<T, A...>::value>>
		explicit Handle(A &&... args) { static_assert(!is_abstract<T>::value, "Can't construct an abstract class"); }

		void keep()
		{
			if(_shared != nullptr)
				++_shared->_refs;
		}

		void release()
		{
			if(_shared != nullptr)
			{
				--_shared->_refs;

				if(_shared->_refs <= 0)
					PointerDeleter<T>::free(_shared);
			}
		}

		template<class U, class ... O, useif<based_on<U, T>::value>, skipif<same_types<Owner, Types<O...>>::value>>
		Handle & operator = (const Handle<U, O...> & h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = static_cast<const T *>(h._shared);
			keep();

			return *this;
		}

		template<class U, class ... O, useif<based_on<U, T>::value>, skipif<same_types<Owner, Types<O...>>::value>>
		Handle & operator = (Handle<U, O...> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = static_cast<const T *>(h._shared);
			h._shared = nullptr;

			return *this;
		}

		Handle & operator = (const T * shared)
		{
			release();
			_shared = shared;
			keep();

			return *this;
		}

		template<typename ... A, selectif(0)<can_construct<T, A...>::value>>
		Handle & init(A &&... args)
		{
			release();
			_shared = new T(forward<A>(args)...);

			return *this;
		}

		template<typename ... A, selectif(1)<cant_construct<T, A...>::value, is_abstract<T>::value>>
		void init(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}

		template<class ... A, selectif(0)<can_construct<T, A...>::value>>
		static Handle create(A &&... args)
		{
			T * sh = new T(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A, selectif(1)<cant_construct<T, A...>::value>>
		static void create(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}

	public:
		Handle() : _shared(nullptr) {}
		Handle(nullptr_t) : _shared(nullptr) {}

		Handle(const Handle & h) : _shared(h._shared) { keep(); }
		Handle(Handle && h) : _shared(h._shared) { h._shared = nullptr; }

		~Handle() { release(); }

		bool isNull() const
		{
			return _shared == nullptr;
		}

		Handle & operator = (const Handle & h)
		{
			if(this == &h)
				return *this;

			release();
			_shared = h._shared;
			keep();

			return *this;
		}

		Handle & operator = (Handle && h)
		{
			if(this == &h)
				return *this;

			release();
			_shared = h._shared;
			h._shared = nullptr;

			return *this;
		}

		template<class U, useif<based_on<U, T>::value, not_same_type<U, T>::value>>
		Handle & operator = (const Handle<U, Owner> & h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = static_cast<const T *>(h._shared);
			keep();

			return *this;
		}

		template<class U, useif<based_on<U, T>::value, not_same_type<U, T>::value>>
		Handle & operator = (Handle<U, Owner> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = static_cast<const T *>(h._shared);
			h._shared = nullptr;

			return *this;
		}

		template<class U>
		bool operator == (const Handle<U> & h) const
		{
			return h._shared == _shared;
		}

		bool operator == (const T * shared) const
		{
			return _shared == shared;
		}

		bool operator == (nullptr_t) const
		{
			return _shared == nullptr;
		}

		template <class U>
		bool operator != (const U & x) const
		{
			return !operator == (x);
		}

		template<class U>
		bool operator > (const Handle<U> & h) const
		{
			return _shared > h._shared;
		}

		template<class U>
		bool operator < (const Handle<U> & h) const
		{
			return _shared < h._shared;
		}

		const T * pointer() const
		{
			return _shared;
		}

		operator const T * () const
		{
			return _shared;
		}

		size_t refs() const
		{
			return _shared->_refs;
		}

		size_t hash() const
		{
			return ptr_hash<T>(inptr_());
		}

		template<class U>
		static Handle cast(const Handle<const U, Owner> & h)
		{
			return static_cast<const T *>(h._shared);
		}
	};

	typedef Handle<Shared> VoidHandle;
	typedef Handle<SharedEmpty> EmptyHandle;

	template<class T, class ... A, selectif(0)<can_construct<T, A...>::value>>
	Handle<T> handle(A &&... args)
	{
		return Handle<T>::create(forward<A>(args)...);
	}

	template<class T, class ... A, selectif(1)<cant_construct<T, A...>::value>>
	void handle(A &&... args)
	{
		static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
	}

	template<class T, class U>
	Handle<T> handle_cast(const Handle<U> & h)
	{
		return Handle<T>::cast(h);
	}

	template<class T, class U, class Owner>
	Handle<T, Owner> handle_cast(const Handle<U, Owner> & h)
	{
		return Handle<T, Owner>::cast(h);
	}

	template<class T, useif<is_shareable<T>::value>>
	Handle<T> share(T * x)
	{
		return x;
	}

	template<class T>
	Handle<T> share(T & x)
	{
		return &x;
	}

	template<class T>
	void share(T && x)
	{
		static_assert(!is_same<T, T>::value, "RaptureCore error: Can't share the temporary object!");
	}

	class SharedIdentifier : public EmptyHandle
	{
	public:
		SharedIdentifier() : EmptyHandle(nothing) {}
	};
}

namespace std
{
	template<class T>
	use_class_hash(Rapture::Handle<T>);
}

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class T>
	struct is_uhandle;

	template<class T, class H>
	struct is_uhandle_of;

	template<class T, class ... A>
	struct is_uhandle_init;

//---------------------------------------------------------------------------

	template<class ... A>
	class UniqueHandle {};

	template<class T>
	class UniqueHandle<T> : public UniqueHandle<T, Empty>
	{
		typedef UniqueHandle<T, Empty> Base;

	public:
		UniqueHandle() : Base() {}

		UniqueHandle(const UniqueHandle & h) = delete;
		UniqueHandle(UniqueHandle && h) : Base(forward<UniqueHandle>(h)) {}

		UniqueHandle(T * shared) : Base(shared) {}
		UniqueHandle(nullptr_t) : Base(nullptr) {}

		template<class U, class ... A, useif<based_on<U, T>::value, !is_const<U>::value>>
		UniqueHandle(UniqueHandle<U, A...> && h) : Base(forward<UniqueHandle<U, A...>>(h)) {}

		template<class U = T, useif<can_construct<U>::value>>
		UniqueHandle(Empty) : Base(nothing) {}

		template<class ... A, selectif(0)<can_construct<T, A...>::value && (sizeof...(A) > 0)>>
		explicit UniqueHandle(A && ... args) : Base(forward<A>(args)...) {}
		template<class ... A, selectif(1)<is_abstract<T>::value, !is_uhandle_init<T, A...>::value>>
		explicit UniqueHandle(A &&... args) { static_assert(!is_abstract<T>::value, "Can't construct an abstract class"); }

		UniqueHandle & operator = (const UniqueHandle & h) = delete;

		UniqueHandle & operator = (UniqueHandle && h)
		{
			Base::operator = (forward<UniqueHandle>(h));
			return *this;
		}

		template<class U, useif<based_on<U, T>::value, !is_const<U>::value>>
		UniqueHandle & operator = (const UniqueHandle<U> & h)
		{
			Base::operator = (h);
			return *this;
		}

		template<class U, useif<based_on<U, T>::value, !is_const<U>::value>>
		UniqueHandle & operator = (UniqueHandle<U> && h)
		{
			Base::operator = (forward<UniqueHandle<U>>(h));
			return *this;
		}

		UniqueHandle & operator = (T * shared)
		{
			Base::operator = (shared);
			return *this;
		}

		template<typename ... A, useif<can_construct<T, A...>::value>>
		UniqueHandle & init(A &&... args)
		{
			Base::init(forward<A>(args)...);
			return *this;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		static UniqueHandle create(A &&... args)
		{
			return new T(forward<A>(args)...);
		}

		template<class ... A, skipif<can_construct<T, A...>::value || !is_abstract<T>::value>>
		static void create(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}
	};

	/**
	 *  @brief
	 *  Common owned version of the Handle class
	 */
	template<class T, class Owner>
	class UniqueHandle<T, Owner> : public Wrapper<T *, UniqueHandle<T, Owner>>, protected DefaultAllocator
	{
		template<class...>
		friend class UniqueHandle;
		friend Owner;

		// Check that T has been already determined
		static_assert(is_determined_class<T>::value, "Class T hasn't been determined yet");

	protected:
		typedef Wrapper<T *, UniqueHandle<T, Owner>> Base;
		friend Base;

		T * _inner;

		UniqueHandle(UniqueHandle && h) : _inner(h._inner) { h._inner = nullptr; }

		UniqueHandle(const UniqueHandle & h) = delete;
		UniqueHandle(T * ptr) : _inner(ptr) {}

		template<class U, class ... A, useif<based_on<U, T>::value, !is_const<U>::value>>
		UniqueHandle(UniqueHandle<U, A...> && h) : _inner(h._inner) { h._inner = nullptr; }

		template<class U = T, useif<can_construct<U>::value>>
		UniqueHandle(Empty) : _inner(new T()) {}

		template<class ... A, useif<can_construct<T, A...>::value, (sizeof...(A) > 0)>>
		explicit UniqueHandle(A && ... args) : _inner(new T(forward<A>(args)...)) {}
		template<class ... A, selectif(1)<is_abstract<T>::value && !is_uhandle_init<T, A...>::value>>
		explicit UniqueHandle(A &&... args) { static_assert(!is_abstract<T>::value, "Can't construct an abstract class"); }

		UniqueHandle & operator = (const UniqueHandle & h) = delete;

		UniqueHandle & operator = (UniqueHandle && h)
		{
			if(this == &h)
				return *this;

			if(_inner)
				delete _inner;

			_inner = h._inner;
			h._inner = nullptr;

			return *this;
		}

		template<class U, useif<based_on<U, T>::value, !is_const<U>::value>>
		UniqueHandle & operator = (UniqueHandle<U> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			if(_inner)
				delete _inner;

			_inner = static_cast<T *>(h._inner);
			h._inner = nullptr;

			return *this;
		}

		UniqueHandle & operator = (T * ptr)
		{
			if(void_ptr(ptr) == void_ptr(_inner))
				return *this;

			if(_inner)
				delete _inner;

			_inner = ptr;

			return *this;
		}

		template<typename ... A, useif<can_construct<T, A...>::value>>
		UniqueHandle & init(A &&... args)
		{
			if(_inner)
				delete _inner;

			_inner = new T(forward<A>(args)...);

			return *this;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		static UniqueHandle create(A &&... args)
		{
			return new T(forward<A>(args)...);
		}

		template<class ... A, skipif<can_construct<T, A...>::value || !is_abstract<T>::value>>
		static void create(A &&... args)
		{
			static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
		}

	public:
		UniqueHandle() : _inner(nullptr) {}
		UniqueHandle(nullptr_t) : _inner(nullptr) {}

		~UniqueHandle() { if(_inner != nullptr) delete _inner; }

		bool isNull() const
		{
			return _inner == nullptr;
		}

		template<class U>
		bool operator == (const UniqueHandle<U> & h) const
		{
			return _inner == h._inner;
		}

		bool operator == (nullptr_t) const
		{
			return _inner == nullptr;
		}

		template<class U>
		bool operator != (const U & x) const
		{
			return !operator == (x);
		}

		template<class U>
		bool operator > (const UniqueHandle<U> & h) const
		{
			return _inner > h._inner;
		}

		template<class U>
		bool operator < (const UniqueHandle<U> & h) const
		{
			return _inner < h._inner;
		}

		T * pointer() const
		{
			return _inner;
		}

		operator T * () const
		{
			return _inner;
		}
	};

	template<class T, class ... A>
	using Unique = UniqueHandle<T, A...>;

	template<class T, class ... A, useif<can_construct<T, A...>::value>>
	UniqueHandle<T> unique_handle(A &&... args)
	{
		return UniqueHandle<T>::create(forward<A>(args)...);
	}

	template<class T, class ... A, skipif<can_construct<T, A...>::value>>
	void unique_handle(A &&... args)
	{
		static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
	}

//---------------------------------------------------------------------------

	template<class T, class R, class C, class ... A>
	struct method_wrapper<Handle<T>, R(C::*)(A...)>
	{
		typedef R(__thiscall C::*MethodType)(A ...);

		R operator()(A ... args)
		{
			return (object->*method)(forward<A>(args)...);
		}

		T * object;
		MethodType method;
	};

	template<class T, class R, class C, class ... A>
	struct method_wrapper<Handle<T>, R (C::*)(A...) const>
	{
		typedef R(__thiscall C::*MethodType)(A ...) const;

		R operator()(A ... args)
		{
			return (object->*method)(forward<A>(args)...);
		}

		T * object;
		MethodType method;
	};

	template<class T, class R, class C, class ... A>
	struct method_wrapper<Handle<T> &, R(C::*)(A...)>
	{
		typedef R(__thiscall C::*MethodType)(A ...);

		R operator()(A ... args)
		{
			return (object->*method)(forward<A>(args)...);
		}

		T * object;
		MethodType method;
	};

	template<class T, class R, class C, class ... A>
	struct method_wrapper<Handle<T> &, R (C::*)(A...) const>
	{
		typedef R(__thiscall C::*MethodType)(A ...) const;

		R operator()(A ... args)
		{
			return (object->*method)(forward<A>(args)...);
		}

		T * object;
		MethodType method;
	};

//---------------------------------------------------------------------------

	template<class T, class Owner>
	struct VariableInitializer<Handle<T, Owner>>
	{
		static Handle<T, Owner> initialize()
		{
			return {nothing};
		}
	};

	template<class T, class Owner = Empty, class ThreadingModel = CommonThreadingModel>
	struct HandleSingleton : Shared, CopySingleton<Handle<T, Owner>, ThreadingModel> {};

//---------------------------------------------------------------------------

	namespace Internals
	{
		template<class T>
		struct is_handle0 : false_type {};

		template<class ... A>
		struct is_handle0<Handle<A...>> : true_type {};

		template<class T, class U>
		struct is_handle_of1 : false_type {};

		template<class T, class U, class ... A>
		struct is_handle_of1<Handle<U, A...>, T> : based_on<U, T> {};

		template<class T, class U>
		struct is_handle_of0 : false_type {};

		template<class T, class U, class ... A>
		struct is_handle_of0<Handle<U, A...>, T> : is_handle_of1<Handle<decay_t<U>, A...>, T> {};

		template<class T>
		struct is_uhandle0 : false_type {};

		template<class ... A>
		struct is_uhandle0<Handle<A...>> : true_type {};

		template<class T, class U>
		struct is_uhandle_of1 : false_type {};

		template<class T, class U, class ... A>
		struct is_uhandle_of1<Handle<U, A...>, T> : based_on<U, T> {};

		template<class T, class U>
		struct is_uhandle_of0 : false_type {};

		template<class T, class U, class ... A>
		struct is_uhandle_of0<Handle<U, A...>, T> : is_uhandle_of1<Handle<decay_t<U>, A...>, T> {};
	}

	template<class T>
	struct is_handle : Internals::is_handle0<decay_t<T>> {};

	template<class H, class T>
	struct is_handle_of : Internals::is_handle_of0<decay_t<H>, T> {};

	template<class T>
	struct is_uhandle : Internals::is_uhandle0<decay_t<T>> {};

	template<class H, class T>
	struct is_uhandle_of : Internals::is_uhandle_of0<decay_t<H>, T> {};

	template<class T, class ... A>
	struct is_handle_init
	{
		using FirstType = first_t<A...>;

		static const bool value =
			sizeof...(A) == 1 && (
			(based_on<FirstType, T>::value && std::is_pointer<FirstType>::value) ||
			is_handle_of<FirstType, T>::value ||
			same_type<FirstType, nullptr_t>::value ||
			same_type<FirstType, Empty>::value
			);
	};

	template<class T, class ... A>
	struct is_uhandle_init
	{
		using FirstType = first_t<A...>;

		static const bool value =
			sizeof...(A) == 1 && (
			(based_on<FirstType, T>::value && std::is_pointer<FirstType>::value) ||
			is_uhandle_of<FirstType, T>::value ||
			same_type<FirstType, nullptr_t>::value ||
			same_type<FirstType, Empty>::value
			);
	};
}

//---------------------------------------------------------------------------

#define friend_handle											        \
	template<class ...>		  friend class  Rapture::Handle;			\
	template<class ...>		  friend class  Rapture::UniqueHandle;	    \
	template<class>			  friend struct Rapture::PointerDeleter;	\
	template<class, class...> friend struct Rapture::is_constructible;

#define friend_owned_handle(...)							            \
	friend class Rapture::Handle<__VA_ARGS__>;				            \
	friend class Rapture::UniqueHandle<__VA_ARGS__>;		            \
	template<class>	friend struct Rapture::PointerDeleter;	            \
	template<class, class...> friend struct Rapture::is_constructible;

//---------------------------------------------------------------------------
#endif
