//---------------------------------------------------------------------------

#ifndef UNSAFE_BLOCK_POOL_H
#define UNSAFE_BLOCK_POOL_H

//---------------------------------------------------------------------------

#include "PagePool.h"

//---------------------------------------------------------------------------

namespace Rapture
{
#define next_ptr(x, ...) *reinterpret_cast<void * __VA_ARGS__ *>(x)

	template<class id, size_t typeSize = sizeof(id), size_t pageSize = 0x10000, size_t alignment = 8>
	class UnsafeBlockPool : protected PagePool<pageSize>
	{
	public:
		UnsafeBlockPool(const UnsafeBlockPool & pool) = delete;
		UnsafeBlockPool & operator = (const UnsafeBlockPool & pool) = delete;

		virtual ~UnsafeBlockPool() {}

		forceinline void * allocate()
		{
			if(_head == nullptr)
				_head = newPage();

			void * ptr = _head;
			_head = next_ptr(ptr);

			return ptr;
		}

		forceinline void free(void * ptr)
		{
			next_ptr(ptr) = _head;
			_head = ptr;
		}

		static const int blockSize = typeSize;

	private:
		template<class ... A>
		friend class Handle;

		template<class T>
		friend inline T & getsingleton();
		
		UnsafeBlockPool() : _head(newPage()) {}

		void * _head;

		void * newPage()
		{
			static const size_t blockSize = align(typeSize, alignment);
			static const size_t count = pageSize / blockSize;

			void * head = PagePool<pageSize>::allocatePage();

			if(head == nullptr)
				throw std::exception("Allocation error!");

			void * ptr = head;

			for(size_t i = 0; i < count - 1; ++i)
			{
				void * next(reinterpret_cast<char *>(ptr) + blockSize);

				next_ptr(ptr) = next;
				ptr = next;
			}

			next_ptr(ptr) = nullptr;

			return head;
		}
	};
}

//---------------------------------------------------------------------------
#endif
