//---------------------------------------------------------------------------

#pragma once

#ifndef ALIGNED_ALLOCATOR_H
#define ALIGNED_ALLOCATOR_H

//---------------------------------------------------------------------------

#include <core/memory/allocator/default_alloc.h>

//---------------------------------------------------------------------------

namespace asd
{
	struct aligned_alloc : default_alloc
	{
		using default_alloc::operator new;
		using default_alloc::operator delete;
		
		static inline void * operator new(size_t size, size_t alignment) {
			if((alignment & (alignment - 1)) != 0) {
				throw std::runtime_error("Invalid alignment!");
			}

#if !defined(_DEBUG) || !defined(_MSC_VER)
			return boost::alignment::aligned_alloc(size, alignment);
#else
			return _aligned_malloc_dbg(size, alignment, __FILE__, __LINE__);
#endif
		}
		
		static inline void operator delete(void * ptr, size_t alignment) {
			if(ptr != nullptr) {
				aligned_free(ptr);
			}
		}
	};
}

//---------------------------------------------------------------------------
#endif
