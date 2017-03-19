//---------------------------------------------------------------------------

#pragma once

#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

//---------------------------------------------------------------------------

namespace asd
{
	template<typename Pool>
	using pool_t = typename Pool::type;

	template<class Pool>
	class PoolAllocator
	{
	public:
		static inline void * operator new(size_t size)
		{
			if(size != pool_t<Pool>::blockSize)
				return ::operator new(size);

			return pool_t<Pool>::instance().allocate();
		}

		static inline void operator delete(void * ptr, size_t size)
		{
			if(size != pool_t<Pool>::blockSize)
			{
				::operator delete(ptr);
				return;
			}

			if(ptr != nullptr)
				pool_t<Pool>::instance().free(ptr);
		}

		static inline void * operator new(size_t size, const nothrow_t & nt) throw()
		{
			if(size != pool_t<Pool>::blockSize)
				return ::operator new(size, nt);

			return pool_t<Pool>::instance().allocate();
		}

		static inline void operator delete(void * ptr, size_t size, const nothrow_t & nt) throw()
		{
			if(size != pool_t<Pool>::blockSize)
			{
				::operator delete(ptr, nt);
				return;
			}

			if(ptr != nullptr)
				pool_t<Pool>::instance().free(ptr);
		}

		static inline void * operator new(size_t, void * place) throw()
		{
			return place;
		}

		static void operator delete(void *, void *) throw() {}
	};
}

//---------------------------------------------------------------------------
#endif
