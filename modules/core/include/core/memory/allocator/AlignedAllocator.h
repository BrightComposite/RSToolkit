//---------------------------------------------------------------------------

#ifndef ALIGNED_ALLOCATOR_H
#define ALIGNED_ALLOCATOR_H

//---------------------------------------------------------------------------

#include <core/memory/allocator/DefaultAllocator.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class AlignedAllocator : public DefaultAllocator
	{
	public:
		void * operator new(size_t size, size_t alignment)
		{
			if((alignment & (alignment - 1)) == 0)
			{
#ifndef _DEBUG
				return _aligned_malloc(size, alignment);
#else
				return _aligned_malloc_dbg(size, alignment, __FILE__, __LINE__);
#endif
			}
			else
				return nullptr;

		}

		void operator delete(void * ptr, size_t alignment)
		{
			if(ptr != nullptr)
				_aligned_free(ptr);
		}
	};
}

//---------------------------------------------------------------------------
#endif
