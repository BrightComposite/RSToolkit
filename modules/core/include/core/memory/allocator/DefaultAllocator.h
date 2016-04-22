//---------------------------------------------------------------------------

#ifndef DEFAULT_ALLOCATOR_H
#define DEFAULT_ALLOCATOR_H

//---------------------------------------------------------------------------

#include <core/memory/Memory.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	struct DefaultAllocator
	{
		static inline void * operator new(size_t size)
		{
			return _new(size);
		}

		static inline void operator delete(void * ptr, size_t size)
		{
			_delete(ptr);
		}

		static inline void * operator new(size_t size, const nothrow_t & nt) throw()
		{
			return _new(size);
		}

		static inline void operator delete(void * ptr, size_t size, const nothrow_t & nt) throw()
		{
			_delete(ptr);
		}

		static inline void * operator new(size_t, void * place) throw()
		{
			return place;
		}

		static inline void operator delete(void *, void *) throw() {}
	};
}

//---------------------------------------------------------------------------
#endif
