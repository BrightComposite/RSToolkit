//---------------------------------------------------------------------------

#pragma once

#ifndef ALIGNED_ALLOCATOR_H
#define ALIGNED_ALLOCATOR_H

//---------------------------------------------------------------------------

#include <core/memory/allocator/DefaultAllocator.h>

//---------------------------------------------------------------------------

namespace asd
{
	struct AlignedAllocator : DefaultAllocator
	{
		using DefaultAllocator::operator new;
		using DefaultAllocator::operator delete;

		void * operator new(size_t size, size_t alignment) throw()
		{
			if((alignment & (alignment - 1)) != 0) {
				throw std::runtime_error("Invalid alignment!");
			}
				
#if !defined(_DEBUG) || !defined(_MSC_VER)
			return aligned_alloc(size, alignment);
#else
			return _aligned_malloc_dbg(size, alignment, __FILE__, __LINE__);
#endif
		}

		void operator delete(void * ptr, size_t alignment)
		{
			if(ptr != nullptr)
				aligned_free(ptr);
		}
	};
}

//---------------------------------------------------------------------------
#endif
