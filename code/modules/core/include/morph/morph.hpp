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
		constexpr morph_id_t next_morph_id() {
			return T::morph_next_id();
		}
		
		template<class T>
		constexpr morph_id_t get_morph_id() {
			return T::morph_id();
		}

		template<class T>
		constexpr const char * get_morph_name() {
			return T::morph_name();
		}
		
		template <class T>
		struct origin
		{
			using type = typename T::origin;
		};
	}
	
	template <class T>
	using origin_t = typename morph_types::origin<T>::type;
	
	template <class T>
	const morph_id_t morph_id = morph_types::get_morph_id<T>();

	template <class T>
	constexpr const char * morph_name = morph_types::get_morph_name<T>();
}

/**
 *	Marks the Origin class as a root of morphs hierarchy
 */
#define morph_origin(Origin)											\
    static asd::morph_id_t morph_next_id()								\
    {																	\
        static std::atomic<morph_id_t> counter(0);						\
        return counter++;												\
    }																	\
																		\
	template<class>														\
	friend constexpr asd::morph_id_t asd::morph_types::next_morph_id();	\
																		\
	template<class> 													\
	friend struct asd::morph_types::origin;								\
																		\
	template<class> 													\
	friend struct asd::is_morph_type;									\
																		\
	using origin = Origin;												\

/**
 *	Creates morph id for the type.
 */
#define morph_type(.../*morph type*/)									\
    static asd::morph_id_t morph_id()									\
    {																	\
        static asd::morph_id_t i = 										\
			asd::morph_types::next_morph_id<origin_t<__VA_ARGS__>>();	\
        return i;														\
    }																	\
																		\
	template<class>														\
	friend constexpr asd::morph_id_t asd::morph_types::get_morph_id();	\
																		\
	template<class>														\
	friend constexpr const char * asd::morph_types::get_morph_name();	\
																		\
    static constexpr const char * morph_name()							\
    {																	\
        return #__VA_ARGS__;											\
    }																	\

//---------------------------------------------------------------------------
#endif