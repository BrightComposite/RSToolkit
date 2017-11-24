//---------------------------------------------------------------------------

#pragma once

#ifndef ALIGNED_H
#define ALIGNED_H

//---------------------------------------------------------------------------

#include <core/memory/allocator/aligned_alloc.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<class T, int Alignment = 4>
	class alignas(asd::align(sizeof(T), Alignment)) aligned : T, aligned_alloc
	{
	public:
		using T::T;
	};
}

//---------------------------------------------------------------------------
#endif
