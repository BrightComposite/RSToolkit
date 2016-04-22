//---------------------------------------------------------------------------

#ifndef HANDLE_H
#define HANDLE_H

//---------------------------------------------------------------------------

#include <core/memory/BlockPool.h>
#include <core/memory/allocator/DefaultAllocator.h>
#include <core/memory/allocator/PoolAllocator.h>
#include <core/meta/Meta.h>
#include <core/addition/Wrapper.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class ... A>
	class Handle;

	template<class ... A>
	class UniqueHandle;

	//---------------------------------------------------------------------------

#define public_for_handle(...)									\
	template<class...>        friend class Handle;				\
	template<class...>        friend class UniqueHandle;		\
	template<class, class>    friend class std::unique_ptr;		\
	template<class, class...> friend struct IsConstructible;	\
	template<class>           friend struct SharedWrapper;		\
	template<class>           friend struct PointerDeleter;		\

	//---------------------------------------------------------------------------

	template<class T>
	struct PointerDeleter
	{
		void operator()(T * ptr)
		{
			delete ptr;
		}
	};

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
		public_for_handle()
	protected:
		mutable ref_counter_t _refs = 1;
	};

	template<class T>
	class UniquePtr : public std::unique_ptr<T, PointerDeleter<T>>
	{
		typedef std::unique_ptr<T, PointerDeleter<T>> Base;

	public:
		using Base::unique_ptr;

		UniquePtr & operator = (nullptr_t) _NOEXCEPT
		{	
			Base::operator = (nullptr);
			return (*this);
		}

		template<class U>
		UniquePtr & operator = (U && right) _NOEXCEPT
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

	/**
	 *  @brief
	 *	Pointer-wrapping kind of handle. It is used with the objects of
	 *	non-shareable classes.
	 */
	template<class T>
	struct SharedWrapper : public Shared
	{
		template<class, bool>
		friend struct SharedCast;

		SharedWrapper(T * obj) : _inner(obj) {}

		template<class ... A>
		SharedWrapper(A &&... args) : _inner(new T(forward<A>(args)...)) {}

		~SharedWrapper()
		{
			PointerDeleter<T> deleter;
			deleter(_inner);
		}

	protected:
		T * _inner;
	};

	class SharedEmpty : public Empty, public Shared {};

	template<class T>
	using is_shareable = is_base_of<Shared, T>;

//---------------------------------------------------------------------------

	/**
	 *  @brief
	 *	Template struct which provides casts for different kinds of handles.
	 */
	template<class T, bool isShareable = is_shareable<T>::value>
	struct SharedCast
	{
		typedef SharedWrapper<T> SharedType;

		static T * toObj(SharedType * shared)
		{
			return shared->_inner;
		}

		template<class U>
		static SharedType * cast(SharedWrapper<U> * shared)
		{
			return reinterpret_cast<SharedType *>(shared);
		}

		static SharedType * toShared(T & x)
		{
			static_assert(false, "RaptureCore error: Can't make this object shared!");
			return nullptr;
		}
	};

	template<class T>
	struct SharedCast<T, true>
	{
		typedef T SharedType;

		static T * toObj(SharedType * shared)
		{
			return shared;
		}

		template<class U>
		static SharedType * cast(U * shared)
		{
			return static_cast<SharedType *>(shared);
		}

		template<class U>
		static SharedType * cast(SharedWrapper<U> * shared)
		{
			static_assert(false, "RaptureCore error: Can't cast SharedWrapper to shareable object!");
			return nullptr;
		}

		static SharedType * toShared(T & x)
		{
			return &x;
		}
	};

//---------------------------------------------------------------------------

	template<class T>
	struct is_handle;

	template<class T, class H>
	struct is_same_handle;

	template<class T, class ... A>
	struct is_handle_init;

//---------------------------------------------------------------------------

	/**
	 *  @brief
	 *  The Handle class is the very important component in
	 *	memory-management of Rapture Engine. It is used to fully control 
	 *	life-time of objects.
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
	 *  Disowned version of the Handle class which is used more often
	 */
	template<class T>
	class Handle<T> : public Handle<T, Empty>
	{
		typedef Handle<T, Empty> Base;
		typedef typename Base::SharedType SharedType;
		typedef typename Base::CastType CastType;

	public:
		Handle() : Base() {}

		Handle(const Handle & h) : Base(h) {}
		Handle(Handle && h) : Base(forward<Handle>(h)) {}

		Handle(SharedType * shared) : Base(shared) {}

		template<class U, class ... A, useif(based_on(U, T) && !is_const<U>::value)>
		Handle(const Handle<U, A...> & h) : Base(h) {}
		template<class U, class ... A, useif(based_on(U, T) && !is_const<U>::value)>
		Handle(Handle<U, A...> && h) : Base(forward<Handle<U, A...>>(h)) {}

		template<useif(can_construct(T))>
		Handle(Empty emptiness) : Base(emptiness) {}

		template<class ... A, selectif(0, can_construct(T, A...) && sizeof...(A) > 0)>
		explicit Handle(A &&... args) : Base(forward<A>(args)...) {}
		template<class ... A, selectif(1, is_abstract<T>::value && !is_handle_init<T, A...>::value)>
		explicit Handle(A &&... args) { static_assert(false, "Can't construct abstract class"); }

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

		template<class U, class ... A, useif(based_on(U, T) && !is_const<U>::value)>
		Handle & operator = (const Handle<U, A...> & h)
		{
			Base::operator = (h);
			return *this;
		}

		template<class U, class ... A, useif(based_on(U, T) && !is_const<U>::value)>
		Handle & operator = (Handle<U, A...> && h)
		{
			Base::operator = (forward<Handle<U, A...>>(h));
			return *this;
		}

		Handle & operator = (SharedType * shared)
		{
			Base::operator = (shared);
			return *this;
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		Handle & init(A &&... args)
		{
			Base::init(forward<A>(args)...);
			return *this;
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		Handle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		static Handle create(A &&... args)
		{
			SharedType * sh = new SharedType(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		Handle & init(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		Handle & reinit(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		static Handle create(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<class U>
		static Handle cast(const Handle<U> & handle)
		{
			return CastType::cast(handle._shared);
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

		// Check that T has been already determined
		static_assert(!std::is_class<T>::value || std::is_base_of<T, T>::value, "");

	protected:
		typedef Wrapper<T *, Handle<T, Owner>> Base;
		friend Base;

		typedef SharedCast<T> CastType;
		typedef typename CastType::SharedType SharedType;

		SharedType * _shared;

		T * inptr_() const 
		{
			return CastType::toObj(_shared);
		}

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
					delete _shared;
			}
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

		template<class U, class ... A, useif(are_not_same((T, Owner), (U, A...)) && based_on(U, T) && !is_const<U>::value)>
		Handle & operator = (const Handle<U, A...> & h) 
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = CastType::cast(h._shared);
			keep();

			return *this;
		}

		template<class U, class ... A, useif(are_not_same((T, Owner), (U, A...)) && based_on(U, T) && !is_const<U>::value)>
		Handle & operator = (Handle<U, A...> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = CastType::cast(h._shared);
			h._shared = nullptr;

			return *this;
		}

		Handle & operator = (SharedType * shared)
		{
			release();
			_shared = shared;
			keep();

			return *this;
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		Handle & init(A &&... args)
		{
			release();
			_shared = new SharedType(forward<A>(args)...);

			return *this;
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		Handle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		Handle & init(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		Handle & reinit(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

	public:
		Handle() : _shared(nullptr) {}
		Handle(const Handle & h) : _shared(h._shared) { keep(); }
		Handle(Handle && h) : _shared(h._shared) { h._shared = nullptr; }

		Handle(SharedType * shared) : _shared(shared) { keep(); }

		template<useif(can_construct(T))>
		Handle(Empty) : _shared(new SharedType()) {}

		template<class U, class ... A, useif(based_on(U, T) && !is_const<U>::value)>
		Handle(const Handle<U, A...> & h) : _shared(CastType::cast(h._shared)) { keep(); }
		template<class U, class ... A, useif(based_on(U, T) && !is_const<U>::value)>
		Handle(Handle<U, A...> && h) : _shared(CastType::cast(h._shared)) { h._shared = nullptr; }

		template<class ... A, selectif(0, can_construct(T, A...) && sizeof...(A) > 0)>
		explicit Handle(A &&... args) : _shared(new SharedType(forward<A>(args)...)) {}
		template<class ... A, selectif(1, is_abstract<T>::value && !is_handle_init<T, A...>::value)>
		explicit Handle(A &&... args) { static_assert(false, "Can't construct abstract class"); }

        ~Handle() { release(); }

		bool isNull() const
		{
			return _shared == nullptr;
		}

		template<class U>
		bool operator == (const Handle<U> & h) const
		{
			return _shared == h._shared;
		}

		template<class U, selectif(1, !std::is_empty<decltype(declval<T>() > declval<U>())>::value)>
		bool operator > (const Handle<U> & h) const
		{
			auto a = inptr_();
			auto b = h.inptr_();

			return a != nullptr && (b == nullptr || *a > *b);
		}

		template<class U, selectif(1, !std::is_empty<decltype(declval<T>() < declval<U>())>::value)>
		bool operator < (const Handle<U> & h) const
		{
			auto a = inptr_();
			auto b = h.inptr_();

			return a == nullptr || (b != nullptr && *a < *b);
		}

		template<class U, selectif(2, std::is_empty<decltype(declval<T>() > declval<U>())>::value)>
		bool operator > (const Handle<U> & h) const
		{
			return _shared > h._shared;
		}

		template<class U, selectif(2, std::is_empty<decltype(declval<T>() < declval<U>())>::value)>
		bool operator < (const Handle<U> & h) const
		{
			return _shared < h._shared;
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
		auto operator << (U && value) -> decltype(declval<T>() << value)
		{
			return inptr_()->operator << (forward<U>(value));
		}

		template<class U>
		auto operator >> (U && value) -> decltype(declval<T>() >> value)
		{
			return inptr_()->operator >> (forward<U>(value));
		}

		template<class U>
		auto operator << (U && value) const -> decltype(declval<const T>() << value)
		{
			return inptr_()->operator << (forward<U>(value));
		}

		template<class U>
		auto operator >> (U && value) const -> decltype(declval<const T>() >> value)
		{
			return inptr_()->operator >> (forward<U>(value));
		}

		template<class U>
		auto operator [] (U && index) -> decltype(declval<T>()[index])
		{
			return inptr_()->operator [] (index);
		}

		template<class U>
		auto operator [] (U && index) const -> decltype(declval<const T>()[index])
		{
			return inptr_()->operator [] (index);
		}

		operator SharedType * () const 
		{
			return _shared;
		}

		size_t refs() const
		{
			return _shared->_refs;
		}

		template<useif(is_functor<std::hash<T>, const T &>::value)>
		size_t hash() const
		{
			return inptr_() == nullptr ? 0 : std::hash<T>()(*inptr_());
		}

		template<skipif(is_functor<std::hash<T>, const T &>::value)>
		size_t hash() const
		{
			return reinterpret_cast<size_t>(inptr_());
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		static Handle create(A &&... args)
		{
			SharedType * sh = new SharedType(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		static Handle create(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<class U>
		static Handle cast(const Handle<U> & h)
		{
			return CastType::cast(h._shared);
		}
	};

	template<class T>
	class Handle<const T> : public Handle<const T, Empty>
	{
		typedef Handle<const T, Empty> Base;
		typedef typename Base::SharedType SharedType;
		typedef typename Base::RealSharedType RealSharedType;

	public:
		Handle() : Base() {}

		Handle(const Handle & h) : Base(h) {}
		Handle(Handle && h) : Base(forward<Handle>(h)) {}

		Handle(const Handle<T> & h) : Base(h) {}
		Handle(Handle<T> && h) : Base(forward<Handle<T>>(h)) {}

		Handle(SharedType * shared) : Base(shared) {}
		Handle(nullptr_t) : Base(nullptr) {}

		template<class U, class ... A, useif(based_on(U, T))>
		Handle(const Handle<U, A...> & h) : Base(h) {}
		template<class U, class ... A, useif(based_on(U, T))>
		Handle(Handle<U, A...> && h) : Base(forward<Handle<U, A...>>(h)) {}

		template<useif(can_construct(T))>
		Handle(Empty emptiness) : Base(emptiness) {}

		template<class ... A, selectif(0, can_construct(T, A...) && sizeof...(A) > 0)>
		explicit Handle(A &&... args) : Base(forward<A>(args)...) {}
		template<class ... A, selectif(1, is_abstract<T>::value && !is_handle_init<T, A...>::value)>
		explicit Handle(A &&... args) { static_assert(false, "Can't construct abstract class"); }

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

		Handle & operator = (const Handle<T> & h)
		{
			Base::operator = (h);
			return *this;
		}

		Handle & operator = (Handle<T> && h)
		{
			Base::operator = (forward<Handle<T>>(h));
			return *this;
		}

		Handle & operator = (const SharedType * shared)
		{
			Base::operator = (shared);
			return *this;
		}

		template <typename ... A, selectif(0, can_construct(T, A...))>
		Handle & init(A &&... args)
		{
			Base::init(forward<A>(args)...);
			return *this;
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		Handle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		static Handle create(A &&... args)
		{
			auto sh = new RealSharedType(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		Handle & init(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		Handle & reinit(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		static Handle create(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<class U>
		static Handle cast(const Handle<U> & handle)
		{
			return handle._shared;
		}
	};

	template<class T, class Owner>
	class Handle<const T, Owner> : public Wrapper<const T *, Handle<const T, Owner>>, protected DefaultAllocator
	{
		template<class...>
		friend class Handle;

		friend struct Shared;
		friend Owner;

		static_assert(!std::is_class<T>::value || std::is_base_of<T, T>::value, "");

	protected:
		typedef Wrapper<const T *, Handle<const T, Owner>> Base;
		friend Base;

		typedef SharedCast<const T> CastType;
		typedef typename CastType::SharedType SharedType;
		typedef typename SharedCast<T>::SharedType RealSharedType;

		SharedType * _shared;

		const T * inptr_() const
		{
			return CastType::toObj(_shared);
		}

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
					delete _shared;
			}
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

		Handle & operator = (const Handle<T> & h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = h._shared;
			keep();

			return *this;
		}

		Handle & operator = (Handle<T> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = h._shared;
			h._shared = nullptr;

			return *this;
		}

		Handle & operator = (SharedType * shared)
		{
			release();
			_shared = shared;
			keep();

			return *this;
		}

		Handle & operator = (SharedType & shared)
		{
			release();
			_shared = &shared;
			keep();

			return *this;
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		Handle & init(A &&... args)
		{
			release();
			_shared = new RealSharedType(forward<A>(args)...);

			return *this;
		}

		template<typename ... A, selectif(0, can_construct(T, A...))>
		Handle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		Handle & init(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		Handle & reinit(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

	public:
		Handle() : _shared(nullptr) {}

		Handle(const Handle & h) : _shared(h._shared)
		{
			keep();
		}

		Handle(Handle && h) : _shared(h._shared)
		{
			h._shared = nullptr;
		}

		Handle(SharedType * shared) : _shared(shared)
		{
			keep();
		}

		Handle(nullptr_t) : _shared(nullptr) {}

		template<class U, class ... A, useif(based_on(U, T))>
		Handle(const Handle<U, A...> & h) : _shared(CastType::cast(h._shared))
		{
			keep();
		}

		template<class U, class ... A, useif(based_on(U, T))>
		Handle(Handle<U, A...> && h) : _shared(CastType::cast(h._shared))
		{
			h._shared = nullptr;
		}

		template<useif(can_construct(T))>
		Handle(Empty) : _shared(new SharedType()) {}

		template<class ... A, selectif(0, can_construct(T, A...) && sizeof...(A) > 0)>
		explicit Handle(A && ... args) : _shared(new SharedType(forward<A>(args)...)) {}
		template<class ... A, selectif(1, is_abstract<T>::value && !is_handle_init<T, A...>::value)>
		explicit Handle(A &&... args) { static_assert(false, "Can't construct abstract class"); }

		~Handle()
		{
			release();
		}

		bool isNull() const
		{
			return _shared == nullptr;
		}

		template<class U>
		bool operator == (const Handle<U> & h) const
		{
			return (h._shared == nullptr) ? (_shared == nullptr) : h._shared == _shared;
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
		auto operator << (U && value) const -> decltype(declval<T>() << declval<U>())
		{
			return inptr_()->operator << (forward<U>(value));
		}

		template<class U>
		auto operator >> (U && value) const -> decltype(declval<T>() >> declval<U>())
		{
			return inptr_()->operator >> (forward<U>(value));
		}

		operator const SharedType * () const
		{
			return _shared;
		}

		size_t refs() const
		{
			return _shared->_refs;
		}

		template<useif(is_functor<std::hash<T>, const T &>::value)>
		size_t hash() const
		{
			return inptr_() == nullptr ? 0 : std::hash<T>()(*inptr_());
		}

		template<skipif(is_functor<std::hash<T>, const T &>::value)>
		size_t hash() const
		{
			return reinterpret_cast<size_t>(inptr_());
		}

		template<class ... A, selectif(0, can_construct(T, A...))>
		static Handle create(A &&... args)
		{
			SharedType * sh = new RealSharedType(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
		static Handle create(A &&... args)
		{
			static_assert(false, "Can't construct an abstract class");
		}

		template<class U>
		static Handle cast(const Handle<U> & h)
		{
			return h._shared;
		}
	};

	typedef Handle<SharedEmpty> EmptyHandle;

	template<class T, class ... A, selectif(0, can_construct(T, A...))>
	Handle<T> handle(A &&... args)
	{
		return Handle<T>::create(forward<A>(args)...);
	}

	template<class T, class ... A, selectif(1, cant_construct(T, A...) && is_abstract<T>::value)>
	Handle<T> handle(A &&... args)
	{
		static_assert(false, "Can't construct an abstract class");
	}

	template<class T, useif(is_shareable<T>::value)>
	Handle<T> share(T * x)
	{
		return x;
	}

	template<class T>
	Handle<T> share(T & x)
	{
		return SharedCast<T>::toShared(x);
	}

	template<class T>
	Handle<T> share(T && x)
	{
		static_assert(false, "RaptureCore error: Can't share the temporary object!");
		return nullptr;
	}

//---------------------------------------------------------------------------

	template<class ... A>
	class UniqueHandle {};

	template<class T>
	class UniqueHandle<T> : public UniqueHandle<T, Empty>
	{
		typedef UniqueHandle<T, Empty> Base;

	public:
		UniqueHandle() : Base() {}

		template<useif(can_construct(T))>
		UniqueHandle(Empty emptiness) : Base(emptiness) {}

		template<class ... A, useif(can_construct(T, A...))>
		UniqueHandle(tuple<A ...> & args) : Base(args) {}

		template<class ... A, useif(can_construct(T, A...))>
		UniqueHandle(tuple<A ...> && args) : Base(move(args)) {}

		template<class H, class ... A, useif(can_construct(T, H, A...))>
		explicit UniqueHandle(H && head, A && ... tail) : Base(forward<H>(head), forward<A>(tail)...) {}

		UniqueHandle(const UniqueHandle & h) = delete;
		UniqueHandle(UniqueHandle && h) : Base(forward<UniqueHandle>(h)) {}

		UniqueHandle(T * shared) : Base(shared) {}

		template<
			class U, class ... A,
				useif(based_on(U, T) && !is_const<U>::value)
		>
		UniqueHandle(UniqueHandle<U, A...> && h) : Base(forward<UniqueHandle<U, A...>>(h)) {}

		UniqueHandle(nullptr_t) : Base(nullptr) {}

		UniqueHandle & operator = (const UniqueHandle & h) = delete;

		UniqueHandle & operator = (UniqueHandle && h)
		{
			Base::operator = (forward<UniqueHandle>(h));
			return *this;
		}

		template<
			class U,
				useif(based_on(U, T) && !is_const<U>::value)
		>
		UniqueHandle & operator = (const UniqueHandle<U> & h)
		{
			Base::operator = (h);
			return *this;
		}

		template<
			class U,
				useif(based_on(U, T) && !is_const<U>::value)
		>
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

		template<typename ... A, useif(can_construct(T, A...))>
		UniqueHandle & init(A &&... args)
		{
			Base::init(forward<A>(args)...);
			return *this;
		}

		template<typename ... A, useif(can_construct(T, A...))>
		UniqueHandle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<class ... A, useif(can_construct(T, A...))>
		static UniqueHandle create(A &&... args)
		{
			return new T(forward<A>(args)...);
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
		static_assert(!std::is_class<T>::value || std::is_base_of<T, T>::value, "");

	protected:
		typedef Wrapper<T *, UniqueHandle<T, Owner>> Base;
		friend Base;

		T * _inner;

		T * inptr_() const 
		{
			return _inner;
		}

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

		template<
			class U,
				useif(based_on(U, T) && !is_const<U>::value)
		>
		UniqueHandle & operator = (UniqueHandle<U> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			if(_inner)
				delete _inner;

			_inner = h._inner;
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

		template<typename ... A, useif(can_construct(T, A...))>
		UniqueHandle & init(A &&... args)
		{
			if(_inner)
				delete _inner;

			_inner = new T(forward<A>(args)...);

			return *this;
		}

		template<typename ... A, useif(can_construct(T, A...))>
		UniqueHandle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<class ... A,
			size_t ... Inds>
		UniqueHandle(tuple<A ...> & args, integer_sequence<size_t, Inds...>) : _inner(new T(forward<A>(std::get<Inds>(args))...)) {}

		template<class ... A,
			size_t ... Inds>
		UniqueHandle(tuple<A ...> && args, integer_sequence<size_t, Inds...>) : _inner(new T(forward<A>(std::get<Inds>(args))...)) {}

	public:
		UniqueHandle() : _inner(nullptr) {}

		template<useif(can_construct(T))>
		UniqueHandle(Empty) : _inner(new T()) {}

		template<class ... A, useif(can_construct(T, A...))>
		UniqueHandle(tuple<A ...> & args) : UniqueHandle(args, make_integer_sequence<size_t, sizeof...(A)>()) {}

		template<class ... A, useif(can_construct(T, A...))>
		UniqueHandle(tuple<A ...> && args) : UniqueHandle(move(args), make_integer_sequence<size_t, sizeof...(A)>()) {}

		template<class H, class ... A, useif(can_construct(T, H, A...))>
		UniqueHandle(H && head, A && ... tail) : _inner(new T(forward<H>(head), forward<A>(tail)...)) {}

		UniqueHandle(const UniqueHandle & h) = delete;

		UniqueHandle(UniqueHandle && h) : _inner(h._inner)
		{
			h._inner = nullptr;
		}

		UniqueHandle(T * ptr) : _inner(ptr) {}

		template<
			class U, class ... A,
				useif(based_on(U, T) && !is_const<U>::value)
		>
		UniqueHandle(UniqueHandle<U, A...> && h) : _inner(h._inner)
		{
			h._inner = nullptr;
		}

		/**
	 	 *	Nullptr-constructor
		 */
		UniqueHandle(nullptr_t) : _inner(nullptr) {}

		~UniqueHandle()
		{
			delete _inner;
		}

		bool isNull() const
		{
			return _inner == nullptr;
		}

		template<class U>
		bool operator == (const UniqueHandle<U> & h) const
		{
			return _inner == h._inner;
		}

		template<class U, skipif(std::is_empty<decltype(declval<T>() > declval<U>())>::value)>
		bool operator > (const UniqueHandle<U> & h) const
		{
			auto a = _inner;
			auto b = h._inner;

			return a != nullptr && (b == nullptr || *a > *b);
		}

		template<class U, skipif(std::is_empty<decltype(declval<T>() < declval<U>())>::value)>
		bool operator < (const UniqueHandle<U> & h) const
		{
			auto a = _inner;
			auto b = h._inner;

			return a == nullptr || (b != nullptr && *a < *b);
		}

		template<class U, useif(std::is_empty<decltype(declval<T>() > declval<U>())>::value)>
		bool operator > (const UniqueHandle<U> & h) const
		{
			return _inner > h._inner;
		}

		template<class U, useif(std::is_empty<decltype(declval<T>() < declval<U>())>::value)>
		bool operator < (const UniqueHandle<U> & h) const
		{
			return _inner < h._inner;
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
		auto operator << (U && value) -> decltype(declval<T>() << value)
		{
			return _inner->operator << (forward<U>(value));
		}

		template<class U>
		auto operator >> (U && value) -> decltype(declval<T>() >> value)
		{
			return _inner->operator >> (forward<U>(value));
		}

		template<class U>
		auto operator << (U && value) const -> decltype(declval<const T>() << value)
		{
			return _inner->operator << (forward<U>(value));
		}

		template<class U>
		auto operator >> (U && value) const -> decltype(declval<const T>() >> value)
		{
			return _inner->operator >> (forward<U>(value));
		}

		template<class U>
		auto operator [] (U && index) -> decltype(declval<T>()[index])
		{
			return _inner->operator [] (index);
		}

		template<class U>
		auto operator [] (U && index) const -> decltype(declval<const T>()[index])
		{
			return _inner->operator [] (index);
		}

		operator T * ()
		{
			return _inner;
		}

		operator const T * () const
		{
			return _inner;
		}

		template<class ... A, useif(can_construct(T, A...))>
		static UniqueHandle create(A &&... args)
		{
			return new T(forward<A>(args)...);
		}
	};

	template<class T, class ... A, useif(can_construct(T, A...))>
	UniqueHandle<T> unique_handle(A &&... args)
	{
		return UniqueHandle<T>::create(forward<A>(args)...);
	}

	template<class T, class ... A, skipif(!is_abstract<T>::value)>
	UniqueHandle<T> unique_handle(A &&... args)
	{
		static_assert(false, "Can't construct an abstract class");
	}

//---------------------------------------------------------------------------

	template<class T, class R, class C, class ... A>
	struct method_wrapper<Handle<T>, R(C::*)(A...)>
	{
		typedef R(__thiscall C::*MethodType)(A ...);

		R operator()(A ... args)
		{
			return (object->*method)(args...);
		}

		Handle<T> object;
		MethodType method;
	};

	template<class T, class R, class C, class ... A>
	struct method_wrapper<Handle<T>, R(C::*)(A &&...)>
	{
		typedef R(__thiscall C::*MethodType)(A &&...);

		R operator()(A &&... args)
		{
			return (object->*method)(forward<A>(args)...);
		}

		Handle<T> object;
		MethodType method;
	};

	template<class T, class R, class C, class ... A>
	struct method_wrapper<Handle<T>, R (C::*)(A...) const>
	{
		typedef R(__thiscall C::*MethodType)(A ...) const;

		R operator()(A ... args)
		{
			return (object->*method)(args...);
		}

		Handle<T> object;
		MethodType method;
	};

	template<class T, class R, class C, class ... A>
	struct method_wrapper<Handle<T>, R(C::*)(A &&...) const>
	{
		typedef R(__thiscall C::*MethodType)(A &&...) const;

		R operator()(A &&... args)
		{
			return (object->*method)(forward<A>(args)...);
		}

		Handle<T> object;
		MethodType method;
	};

	template<class T, class R, class C, class ... A>
	struct method_wrapper<const Handle<T>, R(C::*)(A...) const>
	{
		typedef R(__thiscall C::*MethodType)(A ...) const;

		R operator()(A ... args)
		{
			return (object->*method)(args...);
		}

		const Handle<T> object;
		MethodType method;
	};

	template<class T, class R, class C, class ... A>
	struct method_wrapper<const Handle<T>, R(C::*)(A &&...) const>
	{
		typedef R(__thiscall C::*MethodType)(A &&...) const;

		R operator()(A &&... args)
		{
			return (object->*method)(forward<A>(args)...);
		}

		const Handle<T> object;
		MethodType method;
	};

//---------------------------------------------------------------------------

	template<class T, class Owner>
	struct VariableInitializer<Handle<T, Owner>>
	{
		static Handle<T, Owner> initialize()
		{
			return {emptiness};
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
		struct is_same_handle1 : false_type {};

		template<class T, class ... A>
		struct is_same_handle1<T, Handle<T, A...>> : true_type {};

		template<class T, class U>
		struct is_same_handle0 : false_type {};

		template<class T, class U, class ... A>
		struct is_same_handle0<T, Handle<U, A...>> : is_same_handle1<T, Handle<decay_t<U>, A...>> {};
	}

	template<class T>
	struct is_handle : Internals::is_handle0<decay_t<T>> {};

	template<class T, class H>
	struct is_same_handle : Internals::is_same_handle0<T, decay_t<H>> {};

	template<class T, class ... A>
	struct is_handle_init
	{
		using FirstType = first_t<A...>;

		static const bool value =
			sizeof...(A) == 1 &&
			(
				is_same_handle<T, FirstType>::value ||
				based_on(FirstType, typename SharedCast<T>::SharedType) ||
				same_type(FirstType, nullptr_t) ||
				same_type(FirstType, Empty)
				);
	};
}

//---------------------------------------------------------------------------
#endif
