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

#define public_for_handle(...)								 \
	template<class...> friend class Handle;					 \
	template<class...> friend class UniqueHandle;			 \
	template<class, class...> friend struct IsConstructible; \

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
     *  The Shared class is used to store the pointer which can be accessed
	 *  by the Handle class. Objects of the Shared class also has the
	 *  reference counter.
	 *
	 *	The Shared class by itself is only used by "shareable" objects.
	 *	A shareable object is an object of a class T which inherits the Shared
	 *	class. Such thing allows to create handles to sheareables without
	 *	wrapping them into SharedWrappers. Create sheareable classes when you
	 *	know that objects of these classes will be handled (you will almost
	 *	always want that).
     */
	class Shared : protected DefaultAllocator
	{
		template<class...>
		friend class Handle;

		template<class...>
		friend class UniqueHandle;

		template<class, class>
		friend class Wrapper;

		template<class>
		friend class SharedWrapper;

		template<class, class>
		friend class std::unique_ptr;

		template<class> friend struct PointerDeleter;

	protected:
		mutable ref_counter_t _refs = 1;

		void keep() const
		{
			++_refs;
		}

		void release() const
		{
			--_refs;

			if(_refs <= 0)
				delete this;
		}

		Shared() {}
		Shared(const Shared &) = delete;
		virtual ~Shared() {}

		Shared & operator = (const Shared &) = delete;
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
	class SharedWrapper : public Shared
	{
		template<class, bool>
		friend struct SharedCast;

		template<class...>
		friend class Handle;

	protected:
		T * _inner;

		SharedWrapper(T * obj) : _inner(obj) {}

		template<class ... A>
		SharedWrapper(A &&... args) : _inner(new T(forward<A>(args)...)) {}

		~SharedWrapper()
		{
			PointerDeleter<T> deleter;
			deleter(_inner);
		}
	};

	/**
	 *  @brief
	 *	Template struct which helps to determine if T is shareable class.
	 */
	template<class _Ty>
	struct get_shareable_base _GET_TYPE_OR_DEFAULT(ShareableBase, Empty);

#define shareable_base(...) typename get_shareable_base<__VA_ARGS__>::type

	template<class T>
	struct is_shareable
	{
		using ShareableBase = shareable_base(decay_t<T>);

		static const bool value = not_same(ShareableBase, Empty);
		
		/*	
		 *	We must check that T does not only declare its ShareableBase but also
		 *	inherits it.
		 */
		static_assert(!value || (value && is_base_of<ShareableBase, decay_t<T>>::value), "Shareable class T must inherit its ShareableBase!");
	};

//---------------------------------------------------------------------------

	class SharedEmpty : public Shared, public Empty
	{
	private:
		friend struct get_shareable_base<SharedEmpty>;
		typedef Shared ShareableBase;
	};

//---------------------------------------------------------------------------

	/**
	 *  @brief
	 *	Inherit it to easily make your class shareable.
	 */
	template<class T>
	class Shareable : public Shared
	{
	private:
		friend struct get_shareable_base<T>;
		typedef Shared ShareableBase;
	};

//---------------------------------------------------------------------------

	/**
	 *  @brief
	 *	Template struct which provides casts for different kinds of handles.
	 */
	template<class T, bool isShareable = is_shareable<T>::value>
	struct SharedCast
	{
		typedef SharedWrapper<T> SharedType;

		static inline T * toObj(SharedType * shared)
		{
			return shared->_inner;
		}

		template<class T_>
		static inline SharedType * cast(SharedWrapper<T_> * shared)
		{
			return reinterpret_cast<SharedType *>(shared);
		}

		static inline SharedType * toShared(T & x)
		{
			static_assert(false, "RaptureCore error: Can't make this object shared!");
			return nullptr;
		}
	};

	template<class T>
	struct SharedCast<T, true>
	{
		typedef T SharedType;

		static inline T * toObj(SharedType * shared)
		{
			return shared;
		}

		template<class T_>
		static inline SharedType * cast(T_ * shared)
		{
			return static_cast<SharedType *>(shared);
		}

		template<class T_>
		static inline SharedType * cast(SharedWrapper<T_> * shared)
		{
			static_assert(false, "RaptureCore error: Can't cast SharedWrapper to shareable object!");
			return nullptr;
		}

		static inline SharedType * toShared(T & x)
		{
			return &x;
		}
	};

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

		template<
			require(
				can_construct(T)
				)>
		Handle(Empty emptiness) : Base(emptiness) {}

		template<class ... A,
			require(
				can_construct(T, A ...)
				)>
		Handle(tuple<A ...> & args) : Base(args) {}

		template<class ... A,
			require(
				can_construct(T, A ...)
				)>
		Handle(tuple<A ...> && args) : Base(move(args)) {}

		template<class H, class ... A,
			require(
				can_construct(T, H, A...)
				)>
		explicit Handle(H && head, A && ... tail) : Base(forward<H>(head), forward<A>(tail)...) {}

		Handle(const Handle & h) : Base(h) {}
		Handle(Handle && h) : Base(forward<Handle>(h)) {}

		Handle(SharedType * shared) : Base(shared) {}
		Handle(SharedType & shared) : Base(shared) {}

		template<class T_, class ... A,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)
		>
		Handle(const Handle<T_, A...> & h) : Base(h) {}

		template<class T_, class ... A,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)
		>
		Handle(Handle<T_, A...> && h) : Base(forward<Handle<T_, A...>>(h)) {}

		Handle(nullptr_t) : Base(nullptr) {}

		inline Handle & operator = (const Handle & h)
		{
			Base::operator = (h);
			return *this;
		}

		inline Handle & operator = (Handle && h)
		{
			Base::operator = (forward<Handle>(h));
			return *this;
		}

		template<class T_,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		inline Handle & operator = (const Handle<T_> & h)
		{
			Base::operator = (h);
			return *this;
		}

		template<class T_,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		inline Handle & operator = (Handle<T_> && h)
		{
			Base::operator = (forward<Handle<T_>>(h));
			return *this;
		}

		inline Handle & operator = (SharedType * shared)
		{
			Base::operator = (shared);
			return *this;
		}

		inline Handle & operator = (SharedType & shared)
		{
			Base::operator = (shared);
			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline Handle & init(A &&... args)
		{
			Base::init(forward<A>(args)...);
			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline Handle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		static inline Handle create(A &&... args)
		{
			SharedType * sh = new SharedType(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		inline Handle & init(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		inline Handle & reinit(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		static inline Handle create(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<class T_>
		static inline Handle cast(const Handle<T_> & handle)
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

		friend class Shared;
		friend Owner;

		// Check that T has been already determined
		static_assert(!std::is_class<T>::value || std::is_base_of<T, T>::value, "");

	protected:
		typedef Wrapper<T *, Handle<T, Owner>> Base;
		friend Base;

		typedef SharedCast<T> CastType;
		typedef typename CastType::SharedType SharedType;

		SharedType * _shared;

		inline T * inptr_()
		{
			return CastType::toObj(_shared);
		}

		inline const T * inptr_() const
		{
			return CastType::toObj(_shared);
		}

		inline void keep()
		{
			if(_shared != nullptr)
				_shared->keep();
		}

		inline void release()
		{
			if(_shared != nullptr)
				_shared->release();
		}

		inline Handle & operator = (const Handle & h)
		{
			if(this == &h)
				return *this;

			release();
			_shared = h._shared;
			keep();

			return *this;
		}

		inline Handle & operator = (Handle && h)
		{
			if(this == &h)
				return *this;

			release();
			_shared = h._shared;
			h._shared = nullptr;

			return *this;
		}

		template<class T_,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		inline Handle & operator = (const Handle<T_> & h) 
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = h._shared;
			keep();

			return *this;
		}

		template<class T_,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		inline Handle & operator = (Handle<T_> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = h._shared;
			h._shared = nullptr;

			return *this;
		}

		inline Handle & operator = (SharedType * shared)
		{
			release();
			_shared = shared;
			keep();

			return *this;
		}

		inline Handle & operator = (SharedType & shared)
		{
			release();
			_shared = &shared;
			keep();

			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline Handle & init(A &&... args)
		{
			release();
			_shared = new SharedType(forward<A>(args)...);

			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline Handle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<class ... A,
			size_t ... Inds>
		Handle(tuple<A ...> & args, integer_sequence<size_t, Inds...>) : _shared(new SharedType(forward<A>(std::get<Inds>(args))...)) {}

		template<class ... A,
			size_t ... Inds>
		Handle(tuple<A ...> && args, integer_sequence<size_t, Inds...>) : _shared(new SharedType(forward<A>(std::get<Inds>(args))...)) {}

	public:
		Handle() : _shared(nullptr) {}

		template<
			require(
				can_construct(T)
				)>
		Handle(Empty) : _shared(new SharedType()) {}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		Handle(tuple<A ...> & args) : Handle(args, make_integer_sequence<size_t, sizeof...(A)>()) {}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		Handle(tuple<A ...> && args) : Handle(move(args), make_integer_sequence<size_t, sizeof...(A)>()) {}

		template<class H, class ... A,
			require(
				can_construct(T, H, A...)
				)>
		Handle(H && head, A && ... tail) : _shared(new SharedType(forward<H>(head), forward<A>(tail)...)) {}

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

		Handle(SharedType & shared) : _shared(&shared)
		{
			keep();
		}

		template<class T_, class ... A,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		Handle(const Handle<T_, A...> & h) : _shared(h._shared)
		{
			keep();
		}

		template<class T_, class ... A,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		Handle(Handle<T_, A...> && h) : _shared(h._shared)
		{
			h._shared = nullptr;
		}

		/**
		 *	Nullptr-constructor
		 */
		Handle(nullptr_t) : _shared(nullptr) {}

        ~Handle()
		{
			release();
        }

		inline bool isNull() const
		{
			return _shared == nullptr;
		}

		template<class T_>
		bool operator == (const Handle<T_> & h) const
		{
			return _shared == h._shared;
		}

		template<class T_, require_i(1, !std::is_empty<decltype(declval<T>() > declval<T_>())>::value)>
		bool operator > (const Handle<T_> & h) const
		{
			auto a = inptr_();
			auto b = h.inptr_();

			return a != nullptr && (b == nullptr || *a > *b);
		}

		template<class T_, require_i(1, !std::is_empty<decltype(declval<T>() < declval<T_>())>::value)>
		bool operator < (const Handle<T_> & h) const
		{
			auto a = inptr_();
			auto b = h.inptr_();

			return a == nullptr || (b != nullptr && *a < *b);
		}

		template<class T_, require_i(2, std::is_empty<decltype(declval<T>() > declval<T_>())>::value)>
		bool operator > (const Handle<T_> & h) const
		{
			return _shared > h._shared;
		}

		template<class T_, require_i(2, std::is_empty<decltype(declval<T>() < declval<T_>())>::value)>
		bool operator < (const Handle<T_> & h) const
		{
			return _shared < h._shared;
		}

		bool operator == (nullptr_t) const
		{
			return _shared == nullptr;
		}

		template<class T_>
		bool operator != (const T_ & x) const
		{
			return !operator == (x);
		}

		template<class T_>
		auto operator << (T_ && value) -> decltype(declval<T>() << value)
		{
			return inptr_()->operator << (forward<T_>(value));
		}

		template<class T_>
		auto operator >> (T_ && value) -> decltype(declval<T>() >> value)
		{
			return inptr_()->operator >> (forward<T_>(value));
		}

		template<class T_>
		auto operator << (T_ && value) const -> decltype(declval<const T>() << value)
		{
			return inptr_()->operator << (forward<T_>(value));
		}

		template<class T_>
		auto operator >> (T_ && value) const -> decltype(declval<const T>() >> value)
		{
			return inptr_()->operator >> (forward<T_>(value));
		}

		template<class T_>
		auto operator [] (T_ && index) -> decltype(declval<T>()[index])
		{
			return inptr_()->operator [] (index);
		}

		template<class T_>
		auto operator [] (T_ && index) const -> decltype(declval<const T>()[index])
		{
			return inptr_()->operator [] (index);
		}

		inline operator SharedType * ()
		{
			return _shared;
		}

		inline operator const SharedType * () const
		{
			return _shared;
		}

		size_t refs() const
		{
			return _shared->_refs;
		}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		static inline Handle create(A &&... args)
		{
			SharedType * sh = new SharedType(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		inline Handle & init(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		inline Handle & reinit(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		static inline Handle create(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<class T_>
		static inline Handle cast(const Handle<T_> & h)
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

		template<
			require(
				can_construct(T)
				)>
		Handle(Empty emptiness) : Base(emptiness) {}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		Handle(tuple<A ...> & args) : Base(args) {}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		Handle(tuple<A ...> && args) : Base(move(args)) {}

		template<class H, class ... A,
			require(
				can_construct(T, H, A...)
				)>
		Handle(H && head, A && ... tail) : Base(forward<H>(head), forward<A>(tail)...) {}

		Handle(const Handle & h) : Base(h) {}
		Handle(Handle && h) : Base(forward<Handle>(h)) {}

		Handle(SharedType * shared) : Base(shared) {}
		Handle(SharedType & shared) : Base(shared) {}

		Handle(const Handle<T> & h) : Base(h) {}
		Handle(Handle<T> && h) : Base(forward<Handle<T>>(h)) {}

		template<class T_, class ... A,
			require(
				based_on(T_, T)
				)>
		Handle(const Handle<T_, A...> & h) : Base(h) {}

		template<class T_, class ... A,
			require(
				based_on(T_, T)
				)>
		Handle(Handle<T_, A...> && h) : Base(forward<Handle<T_, A...>>(h)) {}

		Handle(nullptr_t) : Base(nullptr) {}

		inline Handle & operator = (const Handle & h)
		{
			Base::operator = (h);
			return *this;
		}

		inline Handle & operator = (Handle && h)
		{
			Base::operator = (forward<Handle>(h));
			return *this;
		}

		inline Handle & operator = (const Handle<T> & h)
		{
			Base::operator = (h);
			return *this;
		}

		inline Handle & operator = (Handle<T> && h)
		{
			Base::operator = (forward<Handle<T>>(h));
			return *this;
		}

		inline Handle & operator = (const SharedType * shared)
		{
			Base::operator = (shared);
			return *this;
		}

		inline Handle & operator = (const SharedType & shared)
		{
			Base::operator = (shared);
			return *this;
		}

		template <typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline Handle & init(A &&... args)
		{
			Base::init(forward<A>(args)...);
			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline Handle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		static inline Handle create(A &&... args)
		{
			auto sh = new RealSharedType(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		inline Handle & init(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		inline Handle & reinit(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		static inline Handle create(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<class T_>
		static inline Handle cast(const Handle<T_> & handle)
		{
			return handle._shared;
		}
	};

	template<class T, class Owner>
	class Handle<const T, Owner> : public Wrapper<const T *, Handle<const T, Owner>>, protected DefaultAllocator
	{
		template<class...>
		friend class Handle;

		friend class Shared;
		friend Owner;

		static_assert(!std::is_class<T>::value || std::is_base_of<T, T>::value, "");

	protected:
		typedef Wrapper<const T *, Handle<const T, Owner>> Base;
		friend Base;

		typedef SharedCast<const T> CastType;
		typedef typename CastType::SharedType SharedType;
		typedef typename SharedCast<T>::SharedType RealSharedType;

		SharedType * _shared;

		inline const T * inptr_() const
		{
			return CastType::toObj(_shared);
		}

		inline void keep()
		{
			if(_shared != nullptr)
				_shared->keep();
		}

		inline void release()
		{
			if(_shared != nullptr)
				_shared->release();
		}

		inline Handle & operator = (const Handle & h)
		{
			if(this == &h)
				return *this;

			release();
			_shared = h._shared;
			keep();

			return *this;
		}

		inline Handle & operator = (Handle && h)
		{
			if(this == &h)
				return *this;

			release();
			_shared = h._shared;
			h._shared = nullptr;

			return *this;
		}

		inline Handle & operator = (const Handle<T> & h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = h._shared;
			keep();

			return *this;
		}

		inline Handle & operator = (Handle<T> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			release();
			_shared = h._shared;
			h._shared = nullptr;

			return *this;
		}

		inline Handle & operator = (SharedType * shared)
		{
			release();
			_shared = shared;
			keep();

			return *this;
		}

		inline Handle & operator = (SharedType & shared)
		{
			release();
			_shared = &shared;
			keep();

			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline Handle & init(A &&... args)
		{
			release();
			_shared = new RealSharedType(forward<A>(args)...);

			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline Handle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<class ... A,
			size_t ... Inds,
			require(
				can_construct(T, A...)
				)>
		Handle(tuple<A ...> & args, integer_sequence<size_t, Inds...>) : _shared(new SharedType(forward<A>(std::get<Inds>(args))...)) {}

		template<class ... A,
			size_t ... Inds,
			require(
				can_construct(T, A...)
				)>
		Handle(tuple<A ...> && args, integer_sequence<size_t, Inds...>) : _shared(new SharedType(forward<A>(std::get<Inds>(args))...)) {}

	public:
		Handle() : _shared(nullptr) {}

		template<
			require(
				can_construct(T)
				)>
		Handle(Empty) : _shared(new SharedType()) {}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		Handle(tuple<A ...> & args) : Handle(args, make_integer_sequence<size_t, sizeof...(A)>()) {}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		Handle(tuple<A ...> && args) : Handle(move(args), make_integer_sequence<size_t, sizeof...(A)>()) {}

		template<class H, class ... A,
			require(
				can_construct(T, H, A...)
				)>
		Handle(H && head, A && ... tail) : _shared(new SharedType(forward<H>(head), forward<A>(tail)...)) {}

		Handle(const Handle & h) : _shared(h._shared)
		{
			keep();
		}

		Handle(Handle && h) : _shared(h._shared)
		{
			h._shared = nullptr;
		}

		Handle(const Handle<T, Owner> & h) : _shared(h._shared)
		{
			keep();
		}

		Handle(Handle<T, Owner> && h) : _shared(h._shared)
		{
			h._shared = nullptr;
		}

		Handle(SharedType * shared) : _shared(shared)
		{
			keep();
		}

		Handle(SharedType & shared) : _shared(&shared)
		{
			keep();
		}

		template<class T_, class ... A,
			require(
				based_on(T_, T)
				)>
		Handle(const Handle<T_, A...> & h) : _shared(CastType::cast(h._shared))
		{
			keep();
		}

		template<class T_, class ... A,
			require(
				based_on(T_, T)
				)>
		Handle(Handle<T_, A...> && h) : _shared(CastType::cast(h._shared))
		{
			h._shared = nullptr;
		}

		/**
		 *	Nullptr-constructor
		 */
		Handle(nullptr_t) : _shared(nullptr) {}

		~Handle()
		{
			release();
		}

		inline bool isNull() const
		{
			return _shared == nullptr;
		}

		template<class T_>
		bool operator == (const Handle<T_> & h) const
		{
			return (h._shared == nullptr) ? (_shared == nullptr) : h._shared == _shared;
		}

		template<class T_>
		bool operator > (const Handle<T_> & h) const
		{
			return _shared > h._shared;
		}

		template<class T_>
		bool operator < (const Handle<T_> & h) const
		{
			return _shared < h._shared;
		}

		inline bool operator == (nullptr_t) const
		{
			return _shared == nullptr;
		}

		template <class T_>
		inline bool operator != (const T_ & x) const
		{
			return !operator == (x);
		}

		template<class T_>
		auto operator << (T_ && value) const -> decltype(declval<T>() << declval<T_>())
		{
			return inptr_()->operator << (forward<T_>(value));
		}

		template<class T_>
		auto operator >> (T_ && value) const -> decltype(declval<T>() >> declval<T_>())
		{
			return inptr_()->operator >> (forward<T_>(value));
		}

		inline operator const SharedType * () const
		{
			return _shared;
		}

		size_t refs() const
		{
			return _shared->_refs;
		}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		static inline Handle create(A &&... args)
		{
			SharedType * sh = new RealSharedType(forward<A>(args)...);
			--sh->_refs;

			return sh;
		}

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		inline Handle & init(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		inline Handle & reinit(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<typename ... A,
			require(
				!can_construct(T, A...)
				),
			require(
				std::is_abstract<T>::value
				)>
		static inline Handle create(A &&... args) { static_assert(false, "Can't create handle of abstract class!"); }

		template<class T_>
		static inline Handle cast(const Handle<T_> & h)
		{
			return h._shared;
		}
	};

	typedef Handle<SharedEmpty> EmptyHandle;

	template<class T, class ... A,
		require(
			can_construct(T, A...)
			)>
	inline Handle<T> handle(A &&... args)
	{
		return Handle<T>::create(forward<A>(args)...);
	}

	template<class T,
		require(
			is_shareable<T>::value
			)>
	inline Handle<T> share(T * x)
	{
		return x;
	}

	template<class T>
	inline Handle<T> share(T & x)
	{
		return SharedCast<T>::toShared(x);
	}

	template<class T>
	inline Handle<T> share(T && x)
	{
		static_assert(false, "RaptureCore error: Can't share the temporary object!");
		return nullptr;
	}

	template<class T, class Owner>
	struct VariableInitializer<Handle<T, Owner>>
	{
		static Handle<T, Owner> initialize()
		{
			return {emptiness};
		}
	};

	template<class T, class Owner = Empty, class ThreadingModel = CommonThreadingModel>
	using HandleSingleton = CopySingleton<Handle<T, Owner>, ThreadingModel>;
	
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

	template<class ... A>
	class UniqueHandle {};

	template<class T>
	class UniqueHandle<T> : public UniqueHandle<T, Empty>
	{
		typedef UniqueHandle<T, Empty> Base;

	public:
		UniqueHandle() : Base() {}

		template<
			require(
				can_construct(T)
				)>
		UniqueHandle(Empty emptiness) : Base(emptiness) {}

		template<class ... A,
			require(
				can_construct(T, A ...)
				)>
		UniqueHandle(tuple<A ...> & args) : Base(args) {}

		template<class ... A,
			require(
				can_construct(T, A ...)
				)>
		UniqueHandle(tuple<A ...> && args) : Base(move(args)) {}

		template<class H, class ... A,
			require(
				can_construct(T, H, A...)
				)>
		explicit UniqueHandle(H && head, A && ... tail) : Base(forward<H>(head), forward<A>(tail)...) {}

		UniqueHandle(const UniqueHandle & h) = delete;
		UniqueHandle(UniqueHandle && h) : Base(forward<UniqueHandle>(h)) {}

		UniqueHandle(T * shared) : Base(shared) {}

		template<class T_, class ... A,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)
		>
		UniqueHandle(UniqueHandle<T_, A...> && h) : Base(forward<UniqueHandle<T_, A...>>(h)) {}

		UniqueHandle(nullptr_t) : Base(nullptr) {}

		inline UniqueHandle & operator = (const UniqueHandle & h) = delete;

		inline UniqueHandle & operator = (UniqueHandle && h)
		{
			Base::operator = (forward<UniqueHandle>(h));
			return *this;
		}

		template<class T_,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		inline UniqueHandle & operator = (const UniqueHandle<T_> & h)
		{
			Base::operator = (h);
			return *this;
		}

		template<class T_,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		inline UniqueHandle & operator = (UniqueHandle<T_> && h)
		{
			Base::operator = (forward<UniqueHandle<T_>>(h));
			return *this;
		}

		inline UniqueHandle & operator = (T * shared)
		{
			Base::operator = (shared);
			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline UniqueHandle & init(A &&... args)
		{
			Base::init(forward<A>(args)...);
			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline UniqueHandle & reinit(A &&... args)
		{
			return init(forward<A>(args)...);
		}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		static inline UniqueHandle create(A &&... args)
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

		inline T * inptr_()
		{
			return _inner;
		}

		inline const T * inptr_() const
		{
			return _inner;
		}

		inline UniqueHandle & operator = (const UniqueHandle & h) = delete;

		inline UniqueHandle & operator = (UniqueHandle && h)
		{
			if(this == &h)
				return *this;

			if(_inner)
				delete _inner;

			_inner = h._inner;
			h._inner = nullptr;

			return *this;
		}

		template<class T_,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		inline UniqueHandle & operator = (UniqueHandle<T_> && h)
		{
			if(void_ptr(this) == void_ptr(&h))
				return *this;

			if(_inner)
				delete _inner;

			_inner = h._inner;
			h._inner = nullptr;

			return *this;
		}

		inline UniqueHandle & operator = (T * ptr)
		{
			if(void_ptr(ptr) == void_ptr(_inner))
				return *this;

			if(_inner)
				delete _inner;

			_inner = ptr;

			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline UniqueHandle & init(A &&... args)
		{
			if(_inner)
				delete _inner;

			_inner = new T(forward<A>(args)...);

			return *this;
		}

		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		inline UniqueHandle & reinit(A &&... args)
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

		template<
			require(
				can_construct(T)
				)>
		UniqueHandle(Empty) : _inner(new T()) {}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		UniqueHandle(tuple<A ...> & args) : UniqueHandle(args, make_integer_sequence<size_t, sizeof...(A)>()) {}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		UniqueHandle(tuple<A ...> && args) : UniqueHandle(move(args), make_integer_sequence<size_t, sizeof...(A)>()) {}

		template<class H, class ... A,
			require(
				can_construct(T, H, A...)
				)>
		UniqueHandle(H && head, A && ... tail) : _inner(new T(forward<H>(head), forward<A>(tail)...)) {}

		UniqueHandle(const UniqueHandle & h) = delete;

		UniqueHandle(UniqueHandle && h) : _inner(h._inner)
		{
			h._inner = nullptr;
		}

		UniqueHandle(T * ptr) : _inner(ptr) {}

		template<class T_, class ... A,
			require(
				!is_const<T_>::value &&
				based_on(T_, T)
				)>
		UniqueHandle(UniqueHandle<T_, A...> && h) : _inner(h._inner)
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

		inline bool isNull() const
		{
			return _inner == nullptr;
		}

		template<class T_>
		bool operator == (const UniqueHandle<T_> & h) const
		{
			return _inner == h._inner;
		}

		template<class T_, require_i(1, !std::is_empty<decltype(declval<T>() > declval<T_>())>::value)>
		bool operator > (const UniqueHandle<T_> & h) const
		{
			auto a = _inner;
			auto b = h._inner;

			return a != nullptr && (b == nullptr || *a > *b);
		}

		template<class T_, require_i(1, !std::is_empty<decltype(declval<T>() < declval<T_>())>::value)>
		bool operator < (const UniqueHandle<T_> & h) const
		{
			auto a = _inner;
			auto b = h._inner;

			return a == nullptr || (b != nullptr && *a < *b);
		}

		template<class T_, require_i(2, std::is_empty<decltype(declval<T>() > declval<T_>())>::value)>
		bool operator > (const UniqueHandle<T_> & h) const
		{
			return _inner > h._inner;
		}

		template<class T_, require_i(2, std::is_empty<decltype(declval<T>() < declval<T_>())>::value)>
		bool operator < (const UniqueHandle<T_> & h) const
		{
			return _inner < h._inner;
		}

		bool operator == (nullptr_t) const
		{
			return _inner == nullptr;
		}

		template<class T_>
		bool operator != (const T_ & x) const
		{
			return !operator == (x);
		}

		template<class T_>
		auto operator << (T_ && value) -> decltype(declval<T>() << value)
		{
			return _inner->operator << (forward<T_>(value));
		}

		template<class T_>
		auto operator >> (T_ && value) -> decltype(declval<T>() >> value)
		{
			return _inner->operator >> (forward<T_>(value));
		}

		template<class T_>
		auto operator << (T_ && value) const -> decltype(declval<const T>() << value)
		{
			return _inner->operator << (forward<T_>(value));
		}

		template<class T_>
		auto operator >> (T_ && value) const -> decltype(declval<const T>() >> value)
		{
			return _inner->operator >> (forward<T_>(value));
		}

		template<class T_>
		auto operator [] (T_ && index) -> decltype(declval<T>()[index])
		{
			return _inner->operator [] (index);
		}

		template<class T_>
		auto operator [] (T_ && index) const -> decltype(declval<const T>()[index])
		{
			return _inner->operator [] (index);
		}

		inline operator T * ()
		{
			return _inner;
		}

		inline operator const T * () const
		{
			return _inner;
		}

		template<class ... A,
			require(
				can_construct(T, A...)
				)>
		static inline UniqueHandle create(A &&... args)
		{
			return new T(forward<A>(args)...);
		}
	};

	namespace Internals
	{
		template<class T>
		struct _is_handle
		{
			static const bool value = false;
		};

		template<class ... A>
		struct _is_handle<Handle<A...>>
		{
			static const bool value = true;
		};
	}

	template<class T>
	struct is_handle : Internals::_is_handle<decay_t<T>> {};

	template<class T, class ... A,
		require(
			can_construct(T, A...)
			)>
	inline UniqueHandle<T> unique_handle(A &&... args)
	{
		return UniqueHandle<T>::create(forward<A>(args)...);
	}
}

//---------------------------------------------------------------------------
#endif
