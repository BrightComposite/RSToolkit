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
		template <class T>
		struct origin
		{
			using type = typename T::origin;
		};
		
		template<class T>
		morph_id_t next_morph_id() {
            static std::atomic<morph_id_t> counter(0);
            return counter++;
		}
		
		template<class T>
		morph_id_t get_morph_id() {
			static asd::morph_id_t i = asd::morph_types::next_morph_id<typename origin<T>::type>();
			return i;
		}
	}
	
	template <class T>
	using origin_t = typename morph_types::origin<T>::type;
	
	template <class T>
	const morph_id_t morph_id = morph_types::get_morph_id<T>();
}

/**
 *	Marks the Origin class as a root of morphs hierarchy
 */
#define morph_origin(Origin)											\
	template<class> 													\
	friend struct asd::morph_types::origin;								\
																		\
	using origin = Origin;												\

//---------------------------------------------------------------------------
#endif