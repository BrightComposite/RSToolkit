//---------------------------------------------------------------------------

#pragma once

#ifndef MORPHER_HPP
#define MORPHER_HPP

//---------------------------------------------------------------------------

#include <meta/Sfinae.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class Base>
	struct MorphPool {};

	template<class T>
	struct MorphType {};

	type_checker(is_morph_type, MorphBase);
}

/**
 *	Marks the Base class as a root of morphers hierarchy. But you'll still
 *	need to create a pool for morphers based on this root with
 *	'create_morph_pool' macro.
 */
#define morph_base(Base) using MorphBase = Base; template<class> friend struct MorphType; template<class> friend struct is_morph_type

/**
 *	Creates pool of morphers based on the Base class.
 */
#define create_morph_pool(module, .../*morph base*/)											\
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
		using Base = typename __VA_ARGS__::MorphBase;		                                    \
		using Pool = MorphPool<Base>;				                                            \
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