//---------------------------------------------------------------------------

#ifndef BLOCK_POOL_H
#define BLOCK_POOL_H

//---------------------------------------------------------------------------

#include <meta/Types.h>
#include <core/addition/Singleton.h>
#include "Memory.h"

#include <atomic>
#include <mutex>

//---------------------------------------------------------------------------

using std::atomic;

namespace Rapture
{
#define erased_block 0xffffffff

	/**
	 *	@brief
	 *	Safe and lock-free memory pool. Allows allocation of multiply
	 *	elements simultaneously (the number of elements is defined by <count>)
	 *  Must be a singleton. <id> makes every pool unique.
	 *	<blocksCount> MUST be a power of 2 and MUST NOT be greater than
	 *	0x10000 because of memory optimization of indices.
	 *
	 *	Each pool allocates <blocksCount> * <count> * sizeof(<type>) bytes.
	 *	If there is more memory needed, next pool is allocated automatically.
	 */

	template<class id, class type = id, uint16_t count = 1, uint32_t blocksCount = 0x1000>
	class BlockPool : protected std::mutex, public Singleton<BlockPool<id, type, count, blocksCount>>
	{
		friend Singleton<BlockPool<id, type, count, blocksCount>>;

	public:
		type * allocate()
		{
			if(_count.load() <= 0)
				return allocNext();

			int c = --_count;

			if(c <= 0)
			{
				++_count;
				return allocNext();
			}

			uint32_t & place = freeList[++_head % blocksCount];
			while(place == erased_block);

			type * ptr = &buffer[place];
			place = erased_block;

			return ptr;
		}

		void free(void * ptr)
		{
			type * _ptr = reinterpret_cast<type *>(ptr);

			if(_ptr < buffer || _ptr > buffer + lastElement)
			{
				if(_next)
					_next->free(_ptr);

				return;
			}

			freeList[++_tail % blocksCount] = static_cast<uint32_t>(_ptr - buffer);
			++_count;
		}

		static const size_t blockSize = sizeof(type) * count;

		BlockPool(const BlockPool & pool) = delete;
		BlockPool & operator = (const BlockPool & pool) = delete;

	private:
		template<class ... A>
		friend class Handle;

		template<class T>
		friend inline T & getsingleton();

		BlockPool()
		{
			buffer = Memory<type>::allocate(blocksCount * count);

			for(register uint16_t i = 0; i <= lastBlock; ++i)
				freeList[i] = i * count;
		}

		~BlockPool()
		{
			Memory<type>::free(buffer);

			if(_next != nullptr)
				delete _next;
		}
		
		static const uint16_t lastBlock = static_cast<uint16_t>(blocksCount - 1);
		static const uint32_t lastElement = lastBlock * blockSize;

		type * buffer;
		uint32_t freeList[blocksCount];
		atomic<uint16_t> _count = blocksCount;
		atomic<uint16_t> _head = 0;
		atomic<uint16_t> _tail = 0;
		BlockPool * _next = nullptr;

		type * allocNext()
		{
			if(_next == nullptr)
			{
				lock();

				if(_next == nullptr)
					_next = new BlockPool();

				unlock();
			}

			return _next->allocate();
		}
	};
}

//---------------------------------------------------------------------------
#endif
