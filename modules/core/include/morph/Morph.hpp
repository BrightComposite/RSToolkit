//---------------------------------------------------------------------------

#pragma once

#ifndef MORPHER_HPP
#define MORPHER_HPP

//---------------------------------------------------------------------------

#include <meta/Sfinae.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<class Origin>
	struct MorphPool {};

	template<class T>
	struct MorphType {};

	type_checker(is_morph_type, MorphOrigin);
}

/**
 *	Marks the Origin class as a root of morphs hierarchy. But you'll still
 *	need to create a pool for morphs based on this root with
 *	'create_morph_pool' macro.
 */
#define morph_origin(Origin) using MorphOrigin = Origin; template<class> friend struct MorphType; template<class> friend struct is_morph_type

/**
 *	Creates pool of morphs based on the Base class.
 */
#define create_morph_pool(module, .../*morph origin*/)											\
	template<>										                                            \
	struct api(module) MorphPool<__VA_ARGS__>		                                            \
	{												                                            \
		static int increment()						                                            \
		{											                                            \
			static std::atomic<int> counter;		                                            \
			return ++counter;						                                            \
		}											                                            \
	}

/**
 *	Creates morpher for the Type.
 */
#define create_morph_type(module, .../*morph type*/)                                            \
	template<>										                                            \
	struct api(module) MorphType<__VA_ARGS__>				                                    \
	{												                                            \
		using Origin = typename __VA_ARGS__::MorphOrigin;		                                \
		using Pool = MorphPool<Origin>;				                                            \
													                                            \
		static const char * name()					                                            \
		{											                                            \
			return #__VA_ARGS__;																\
		}											                                            \
													                                            \
		static int id()								                                            \
		{											                                            \
			static int i = Pool::increment();		                                            \
			return i;								                                            \
		}											                                            \
	}

//---------------------------------------------------------------------------

#define morphid(.../*morph type*/) MorphType<__VA_ARGS__>::id()

//---------------------------------------------------------------------------
#endif