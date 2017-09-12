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
	using morph_id_t = size_t;
	
	template <class Origin>
	struct morph_pool {};
	
	template <class T>
	struct morph_type
	{
		static morph_id_t id();
	};
	
	type_checker(is_morph_type, origin);
	
	template <class Origin, class T>
	using is_morph_of = is_true<is_morph_type<T>::value, is_same<typename T::origin, Origin>::value>;
	
	template <class T>
	const morph_id_t morph_id = morph_type<T>::id();
}

/**
 *	Marks the Origin class as a root of morphs hierarchy. But you'll still
 *	need to create a pool for morphs based on this root with
 *	'create_morph_pool' macro.
 */
#define morph_origin(Origin) using origin = Origin; template<class> friend struct morph_type; template<class> friend struct is_morph_type

/**
 *	Creates pool of morphs based on the base_type class.
 */
#define create_morph_pool(module, .../*morph origin*/)                                          \
    template<>                                                                                  \
    struct api(module) morph_pool<__VA_ARGS__>                                                  \
    {                                                                                           \
        static morph_id_t increment()                                                           \
        {                                                                                       \
            static std::atomic<morph_id_t> counter(0);                             				\
            return counter++;                                                                   \
        }                                                                                       \
    }

/**
 *	Creates morpher for the Type.
 */
#define create_morph_type(module, .../*morph type*/)                                            \
    template<>                                                                                  \
    struct api(module) morph_type<__VA_ARGS__>                                                  \
    {                                                                                           \
        using origin = typename __VA_ARGS__::origin;                                            \
        using pool = morph_pool<origin>;                                                        \
                                                                                                \
        static const char * name()                                                              \
        {                                                                                       \
            return #__VA_ARGS__;                                                                \
        }                                                                                       \
                                                                                                \
        static morph_id_t id()                                                                  \
        {                                                                                       \
            static morph_id_t i = pool::increment();                                            \
            return i;                                                                           \
        }                                                                                       \
    }

//---------------------------------------------------------------------------
#endif