//---------------------------------------------------------------------------

#pragma once

#ifndef CONTENTS_H
#define CONTENTS_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace Internals
	{
		template<class T>
		struct Contents {};
	}

#define aligned_contents(Class, alignment, ... /* contents */)																\
	namespace Internals													                                                    \
	{																	                                                    \
		template<>														                                                    \
		struct alignment Contents<Class> 								                                                    \
		{																                                                    \
			static const int count = pp_seq_fields_count(__VA_ARGS__);	                                                    \
			using types	= Types<pp_seq_fields_types(__VA_ARGS__)>;		                                                    \
																		                                                    \
			pp_seq_fields(__VA_ARGS__)									                                                    \
		};																                                                    \
	}

#define declare_contents(Class, ... /* contents */) aligned_contents(Class, , __VA_ARGS__)

	template<class T>
	using contents_types = typename Internals::Contents<T>::types;

	template<class T, class ... A>
	struct can_construct_contents : bool_type<
		are_convertible<Types<A...>, contents_types<T>>::value ||
		(sizeof...(A) == 0 && can_construct<Internals::Contents<T>>::value)
	> {};

	template<class T>
	struct alignas(alignof(Internals::Contents<T>)) Contents : Internals::Contents<T>
	{
		using Base = Internals::Contents<T>;

		Contents() {}

		template<class ... A, useif<can_construct_contents<T, A...>::value>>
		Contents(A &&... args) :
			Contents(contents_types<T>(), forward<A>(args)...) {}

		void * pointer()
		{
			return reinterpret_cast<void *>(this);
		}

		const void * pointer() const
		{
			return reinterpret_cast<const void *>(this);
		}

		operator void * ()
		{
			return reinterpret_cast<void *>(this);
		}

		operator const void * () const
		{
			return reinterpret_cast<const void *>(this);
		}

	private:
		template<class ... AT, class ... A>
		Contents(Types<AT...>, A &&... args) :
			Base({adapt<AT, A>(forward<A>(args))...}) {}
	};
}

//---------------------------------------------------------------------------
#endif
