//---------------------------------------------------------------------------

#pragma once

#ifndef MORPHER_HPP
#define MORPHER_HPP

//---------------------------------------------------------------------------

#include <meta/sfinae.h>
#include <atomic>

//---------------------------------------------------------------------------

namespace asd
{
    using class_id_t = size_t;
    
    type_checker(has_origin, origin);
    
    namespace detail
    {
        template <class T>
        struct origin
        {
            using type = typename T::origin;
        };
        
        template<class T>
        class_id_t next_class_id() {
            static std::atomic<class_id_t> counter(0);
            return counter++;
        }
        
        template<class T, class Origin>
        class_id_t get_class_id() {
            static asd::class_id_t i = asd::detail::next_class_id<Origin>();
            return i;
        }
    }
    
    template <class T>
    using origin_t = typename detail::origin<T>::type;

    template <class T, class Origin>
    const class_id_t relative_class_id = detail::get_class_id<T, Origin>();

    template <class T>
    const class_id_t class_id = relative_class_id<T, origin_t<T>>;
}

/**
 *	Marks the Origin class as a root of morphs hierarchy
 */
#define origin_class(Origin)											\
    template<class> 													\
    friend struct asd::detail::origin;								    \
                                                                        \
    using origin = Origin;												\

//---------------------------------------------------------------------------
#endif