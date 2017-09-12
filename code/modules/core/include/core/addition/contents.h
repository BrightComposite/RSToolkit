//---------------------------------------------------------------------------

#pragma once

#ifndef CONTENTS_H
#define CONTENTS_H

//---------------------------------------------------------------------------

#include <meta/meta.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace internals
	{
		template<class T>
		struct contents {};
	}

#define aligned_contents(Class, alignment, ... /* contents */)																\
	namespace internals													                                                    \
	{																	                                                    \
		template<>														                                                    \
		struct alignment contents<Class> 								                                                    \
		{																                                                    \
			static const int count = pp_seq_fields_count(__VA_ARGS__);	                                                    \
			using types	= asd::types<pp_seq_fields_types(__VA_ARGS__)>;		                                                    \
																		                                                    \
			pp_seq_fields(__VA_ARGS__)									                                                    \
		};																                                                    \
	}

#define declare_contents(Class, ... /* contents */) aligned_contents(Class, , __VA_ARGS__)

	template<class T>
	using contents_types = typename internals::contents<T>::types;

	template<class T, class ... A>
	struct can_construct_contents : bool_type<
		are_convertible<asd::types<A...>, contents_types<T>>::value ||
		(sizeof...(A) == 0 && can_construct<internals::contents<T>>::value)
	> {};

	template<class T>
	struct alignas(alignof(internals::contents<T>)) contents : internals::contents<T>
	{
		using base = internals::contents<T>;

		contents() {}

		template<class ... A, useif<can_construct_contents<T, A...>::value>>
		contents(A &&... args) :
			contents(contents_types<T>(), forward<A>(args)...) {}

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
		contents(types<AT...>, A &&... args) :
			base({adapt<AT, A>(forward<A>(args))...}) {}
	};
}

//---------------------------------------------------------------------------
#endif
