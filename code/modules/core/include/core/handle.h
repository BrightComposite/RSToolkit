//---------------------------------------------------------------------------

#pragma once

#ifndef HANDLE_H
#define HANDLE_H

//---------------------------------------------------------------------------

#include <core/addition/wrapper.h>
#include <core/shareable.h>
#include <container/container.h>

#include <boost/intrusive_ptr.hpp>
#include <boost/move/unique_ptr.hpp>

//---------------------------------------------------------------------------

namespace asd
{
    template<class T>
    class handle;
    
    template<class T>
    class unique;
    
    //---------------------------------------------------------------------------
    
    template<class T>
    struct is_handle;
    
    template<class T, class H>
    struct is_handle_of;
    
    template<class T, class ... A>
    struct is_handle_init;
    
    template<class T>
    struct is_uhandle;
    
    template<class T, class H>
    struct is_uhandle_of;
    
    template<class T, class ... A>
    struct is_uhandle_init;
    
    //---------------------------------------------------------------------------
    
    /**
     *  @brief
     *  The handle class is the very important component in
     *	memory-management of asd. It is used to fully
     *	control life-time of objects.
     *  It contains functions for allocating, deallocating, referencing and
     *  releasing of objects of the type T.
     *  The objects of this class store only pointer to the object.
     *  They may be statically casted to object-pointer or shared-pointer.
     *  Use operator *() to get object.
     *  Use this class with pointers to shared objects.
     */
    template<class T>
    class handle : public boost::intrusive_ptr<T>, public wrapper<T *, handle<T>>
    {
        using base = boost::intrusive_ptr<T>;
        using base_wrapper = wrapper<T *, handle<T>>;
        
    public:
        handle() noexcept : base() {}
        
        handle(nullptr_t) noexcept : base() {}
        
        handle(const handle & h) : base(h) {}
        
        handle(handle && h) : base(std::forward<handle>(h)) {}
        
        handle(T * ptr) : base(ptr) {}
        
        template<class U, class ... A, useif<based_on<U, T>::value && !is_const<U>::value>>
        handle(const handle<U> & h) : base(h) {}
        
        template<class U, class ... A, useif<based_on<U, T>::value && !is_const<U>::value>>
        handle(handle<U> && h) : base(std::forward<handle<U>>(h)) {}
        
        template<class U = T, useif<can_construct<U>::value>>
        handle(empty) : base(new T()) {}
        
        template<class ... A, selectif(0)<can_construct<T, A...>::value, (sizeof...(A) > 0)> >
        explicit handle(A && ... args) : base(new T(std::forward<A>(args)...)) {}
        
        template<class ... A, selectif(1)<is_abstract<T>::value, !is_handle_init<T, A...>::value> >
        explicit handle(A && ... args) {
            static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
        }
        
        handle & operator =(const handle & h) {
            handle(h).swap(*this);
            return *this;
        }
        
        template<class U, useif<based_on<U, T>::value, !is_const<U>::value>>
        handle & operator =(const handle<U> & h) {
            handle(h).swap(*this);
            return *this;
        }
        
        handle & operator =(handle && h) {
            handle(std::forward<handle>(h)).swap(*this);
            return *this;
        }
        
        template<class U, useif<based_on<U, T>::value, !is_const<U>::value>>
        handle & operator =(handle<U> && h) {
            handle(std::forward<handle<U>>(h)).swap(*this);
            return *this;
        }
        
        handle & operator =(T * ptr) {
            handle(ptr).swap(*this);
            return *this;
        }
        
        using base::operator ->;
        using base::operator *;

        using base_wrapper::operator ==;
        using base_wrapper::operator !=;
        using base_wrapper::operator <;
        using base_wrapper::operator >;
        
        T * pointer() const {
            return this->get();
        }
        
        size_t refs() const {
            return pointer()->_refs;
        }
        
        template<typename ... A, selectif(0)<can_construct<T, A...>::value> >
        handle & init(A && ... args) {
            this->reset(new T(std::forward<A>(args)...));
            return *this;
        }
        
        template<typename ... A, selectif(0)<can_construct<T, A...>::value> >
        static handle create(A && ... args) {
            T * ptr = new T(std::forward<A>(args)...);
            --ptr->_refs;
            
            return ptr;
        }
        
        template<typename ... A, selectif(1)<cant_construct<T, A...>::value, is_abstract<T>::value> >
        void init(A && ... args) {
            static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
        }
        
        template<typename ... A, selectif(1)<cant_construct<T, A...>::value, is_abstract<T>::value> >
        static void create(A && ... args) {
            static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
        }
        
        template<class U, useif<std::is_convertible<U *, T *>::value>>
        static handle cast(const handle<U> & h) {
            return static_cast<T *>(h.pointer());
        }
        
        template<class U, useif<std::is_convertible<U *, T *>::value>>
        static handle cast(handle<U> && h) {
            handle out = static_cast<T *>(h.pointer());
            h = nullptr;
            return out;
        }
    };
    
    //---------------------------------------------------------------------------
    
    template<class T>
    class unique : public boost::movelib::unique_ptr<T>
    {
        using base = boost::movelib::unique_ptr<T>;
    
    public:
        unique() throw() : base() {}
        
        unique(const unique & u) throw() = delete;
        
        unique(unique && u) throw() : base(u.release()) {}
        
        unique(T * shared) throw() : base(shared) {}
        
        unique(nullptr_t) throw() : base(nullptr) {}
        
        template<class U, useif<based_on<U, T>::value, !is_const<U>::value>>
        unique(unique<U> && u) throw() : base(u.release()) {}
        
        template<class U = T, useif<can_construct<U>::value>>
        unique(empty) : base(new T()) {}
        
        template<class ... A, selectif(0)<can_construct<T, A...>::value && (sizeof...(A) > 0)> >
        explicit unique(A && ... args) : base(new T(std::forward<A>(args)...)) {}
        
        template<class ... A, selectif(1)<is_abstract<T>::value, !is_uhandle_init<T, A...>::value> >
        explicit unique(A && ... args) { static_assert(!is_abstract<T>::value, "Can't construct an abstract class"); }
        
        template<class U, useif<based_on<U, T>::value, !is_const<U>::value>>
        unique(handle<U> && h) throw() {
            this->reset(h.detach());
        }
        
        unique & operator =(const unique &) = delete;
        
        unique & operator =(unique && u) throw() {
            this->reset(u.release());
            return *this;
        }
        
        template<class U, useif<is_base_of<T, U>::value, !is_const<U>::value>>
        unique & operator =(unique<U> && u) throw() {
            this->reset(u.release());
            return *this;
        }
        
        template<class U, useif<is_base_of<T, U>::value, !is_const<U>::value>>
        unique & operator =(handle<U> && h) throw() {
            this->reset(h.detach());
            return *this;
        }
        
        unique & operator =(T * ptr) throw() {
            this->reset(ptr);
            return *this;
        }
        
        template<typename ... A, useif<can_construct<T, A...>::value>>
        unique & init(A && ... args) {
            this->reset(new T(std::forward<A>(args)...));
            return *this;
        }
        
        operator T * () const {
            return this->get();
        }
        
        operator bool () const {
            return this->get() != nullptr;
        }
        
        template<class ... A, useif<can_construct<T, A...>::value>>
        static unique create(A && ... args) {
            return new T(std::forward<A>(args)...);
        }
        
        template<class ... A, skipif<can_construct<T, A...>::value || !is_abstract<T>::value>>
        static void create(A && ... args) {
            static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
        }
    };
    
    //---------------------------------------------------------------------------
    
    namespace make {
        template<class T, class ... A, selectif(0)<can_construct<T, A...>::value>>
        asd::handle<T> handle(A && ... args) {
            return asd::handle<T>::create(std::forward<A>(args)...);
        }
        
        template<class T, class ... A, selectif(1)<cant_construct<T, A...>::value >>
        void handle(A && ...) {
            static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
        }
        
        template<class T, class ... A, selectif(0)<can_construct<T, A...>::value>>
        asd::unique<T> unique(A && ... args) {
            return asd::unique<T>::create(std::forward<A>(args)...);
        }
        
        template<class T, class ... A, skipif<can_construct<T, A...>::value>>
        void unique(A && ...) {
            static_assert(!is_abstract<T>::value, "Can't construct an abstract class");
        }
    }
    
    template<class T, useif<is_shareable<T>::value>>
    handle<T> share(T * x) {
        return x;
    }
    
    template<class T>
    handle<T> share(T & x) {
        return &x;
    }
    
    template<class T, class U = T *>
    void share(T && x) {
        static_assert(!is_pointer<U>::value, "asd::core error: Can't share the temporary object!");
    }
    
    struct shareable_empty : shareable<shareable_empty> {};

    typedef handle<shareable_empty> empty_handle_t;
    
    class shared_id_t : public empty_handle_t
    {
    public:
        shared_id_t() : empty_handle_t(_) {}
    };
    
    template<class ... A>
    use_class_hash(handle<A...>);
    
    template<class T>
    struct is_uhandle;
    
    template<class T, class H>
    struct is_uhandle_of;
    
    template<class T, class ... A>
    struct is_uhandle_init;
    
    //---------------------------------------------------------------------------
    
    template<typename T, typename R, typename C, typename ... A>
    struct method_wrapper<handle<T>, R(C::*)(A...)>
    {
        typedef R(__thiscall C::*MethodType)(A...);
        
        R operator ()(A ... args) {
            return (object->*method)(args...);
        }
        
        handle<T> object;
        MethodType method;
    };
    
    template<typename T, typename R, typename C, typename ... A>
    struct method_wrapper<handle<T>, R(C::*)(A...) const>
    {
        typedef R(__thiscall C::*MethodType)(A...) const;
        
        R operator ()(A ... args) const {
            return (object->*method)(forward<A>(args)...);
        }
        
        handle<T> object;
        MethodType method;
    };
    
    template<typename T, typename R, typename C, typename ... A>
    struct method_wrapper<handle<T> &, R(C::*)(A...)>
    {
        typedef R(__thiscall C::*MethodType)(A...);
        
        R operator ()(A ... args) {
            return (object->*method)(args...);
        }
        
        handle<T> object;
        MethodType method;
    };
    
    template<typename T, typename R, typename C, typename ... A>
    struct method_wrapper<handle<T> &, R(C::*)(A...) const>
    {
        typedef R(__thiscall C::*MethodType)(A...) const;
        
        R operator ()(A ... args) const {
            return (object->*method)(forward<A>(args)...);
        }
        
        handle<T> object;
        MethodType method;
    };
    
    template<typename T, typename R, typename C, typename ... A>
    struct method_wrapper<const handle<T> &, R(C::*)(A...)>
    {
        typedef R(__thiscall C::*MethodType)(A...);
        
        R operator ()(A ... args) {
            return (object->*method)(args...);
        }
        
        handle<T> object;
        MethodType method;
    };
    
    template<typename T, typename R, typename C, typename ... A>
    struct method_wrapper<const handle<T> &, R(C::*)(A...) const>
    {
        typedef R(__thiscall C::*MethodType)(A...) const;
        
        R operator ()(A ... args) const {
            return (object->*method)(forward<A>(args)...);
        }
        
        handle<T> object;
        MethodType method;
    };
}

_useif_class(sfinae_handle);

#define handleif            _useif(sfinae_handle)
#define handleif_t            _useif_t(sfinae_handle)

template<bool ... Others>
struct use_filter_t<sfinae_handle, Others...> : use_filter_t<sfinae_use, Others...> {};

namespace asd
{
    namespace cast
    {
        template<class T, class Y, handleif<(based_on<T, Y>::value || based_on<Y, T>::value)>>
        inline handle<T> as(const handle<Y> & x) {
            return static_cast<T *>(static_cast<Y *>(x));
        }
        
        template<class T, class Y, handleif<(based_on<T, Y>::value || based_on<Y, T>::value)>>
        inline handle<T> as(handle<Y> && x) {
            handle<T> out = static_cast<T *>(static_cast<Y *>(x));
            x = nullptr;
            return out;
        }
    }
    
    //---------------------------------------------------------------------------
    
    namespace internals
    {
        template<class T>
        struct is_handle0 : false_type {};
        
        template<class T>
        struct is_handle0<handle<T>> : true_type {};
        
        template<class T, class U>
        struct is_handle_of1 : false_type {};
        
        template<class T, class U>
        struct is_handle_of1<handle<U>, T> : based_on<U, T> {};
        
        template<class T, class U>
        struct is_handle_of0 : false_type {};
        
        template<class T, class U>
        struct is_handle_of0<handle<U>, T> : is_handle_of1<handle<decay_t<U>>, T> {};
        
        template<class T>
        struct is_uhandle0 : false_type {};
        
        template<class T>
        struct is_uhandle0<unique<T>> : true_type {};
        
        template<class T, class U>
        struct is_uhandle_of1 : false_type {};
        
        template<class T, class U>
        struct is_uhandle_of1<unique<U>, T> : based_on<U, T> {};
        
        template<class T, class U>
        struct is_uhandle_of0 : false_type {};
        
        template<class T, class U>
        struct is_uhandle_of0<unique<U>, T> : is_uhandle_of1<unique<decay_t<U>>, T> {};
    }
    
    template<class T>
    struct is_handle : internals::is_handle0<decay_t<T>> {};
    
    template<class H, class T>
    struct is_handle_of : internals::is_handle_of0<decay_t<H>, T> {};
    
    template<class T>
    struct is_uhandle : internals::is_uhandle0<decay_t<T>> {};
    
    template<class H, class T>
    struct is_uhandle_of : internals::is_uhandle_of0<decay_t<H>, T> {};
    
    template<class T, class ... A>
    struct is_handle_init
    {
        using FirstType = first_t<A...>;
        
        static const bool value =
            sizeof...(A) == 1 && (
                (based_on<FirstType, T>::value && std::is_pointer<FirstType>::value) ||
                    is_handle_of<FirstType, T>::value ||
                    same_type<FirstType, nullptr_t>::value ||
                    same_type<FirstType, empty>::value
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
                    same_type<FirstType, empty>::value
            );
    };
}

//---------------------------------------------------------------------------

#define friend_handle                                                    \
    template<class>         friend class  ::asd::handle;                \
    template<class>         friend class  ::asd::unique;        \
    template<class>             friend struct ::asd::pointer_deleter;        \
    template<class, class...> 	friend struct ::asd::is_constructible;

#define friend_owned_handle(T)                                        \
    friend class ::asd::handle<T>;                            \
    friend class ::asd::unique<T>;                        \
    template<class>    friend struct ::asd::pointer_deleter;                \
    template<class, class...> friend struct ::asd::is_constructible;

//---------------------------------------------------------------------------
#endif
