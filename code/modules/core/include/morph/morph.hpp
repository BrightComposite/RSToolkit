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
	
	type_checker(is_morph_type, origin);
	
	template <class Origin, class T>
	using is_morph_of = is_true<is_morph_type<T>::value, is_same<typename T::origin, Origin>::value>;
	
	namespace morph_types
	{
		template<class T>
		constexpr morph_id_t increment() {
			return T::morph_increment();
		}

		template<class T>
		constexpr morph_id_t get_id() {
			return T::morph_id();
		}

		template<class T>
		constexpr const char * get_name() {
			return T::morph_name();
		}
	}

	template <class T>
	const morph_id_t morph_id = morph_types::get_id<T>();

	template <class T>
	constexpr const char * morph_name = morph_types::get_name<T>();
}

/**
 *	Marks the Origin class as a root of morphs hierarchy. But you'll still
 *	need to create a pool for morphs based on this root with
 *	'create_morph_pool' macro.
 */
#define morph_origin(Origin)									\
    static morph_id_t morph_increment()							\
    {															\
        static std::atomic<morph_id_t> counter(0);				\
        return counter++;										\
    }															\
																\
	template<class>												\
	friend morph_id_t asd::morph_types::increment();			\
																\
	template<class> 											\
	friend struct asd::is_morph_type;							\
																\
	using origin = Origin;										\

  /**
   *	Creates morpher for the Type.
   */
#define morph_type(.../*morph type*/)							\
    static morph_id_t morph_id()								\
    {															\
        static morph_id_t i = morph_increment();				\
        return i;												\
    }															\
																\
	template<class>												\
	friend morph_id_t asd::morph_types::get_id();				\
																\
	template<class>												\
	friend constexpr const char * asd::morph_types::get_name();	\
																\
    static constexpr const char * morph_name()					\
    {															\
        return #__VA_ARGS__;									\
    }															\

//---------------------------------------------------------------------------
#endif