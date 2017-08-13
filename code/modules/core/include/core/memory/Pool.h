//---------------------------------------------------------------------------

#pragma once

#ifndef POOL_H
#define POOL_H

//---------------------------------------------------------------------------

#include "memory.h"

#include <atomic>
#include <mutex>

//---------------------------------------------------------------------------

using std::atomic;

namespace asd
{
#define erased_block 0xffffffff

	struct thread_safe_pool {};

	/**
	 *	@brief
	 *	Basic memory pool without thread-safety.
	 *	<blocksCount> MUST be a power of 2 and MUST NOT be greater than
	 *	0x10000 because of memory optimization of indices.
	 *
	 *	Each pool allocates <blocksCount> * sizeof(<type>) bytes.
	 *	If there is more memory needed, next pool is allocated automatically.
	 */
	template<class T, uint blocksCount = 0x1000, class ThreadPolicy = empty>
	class block_pool
	{
		deny_copy(block_pool);

	public:
		block_pool()
		{
			_buffer = memory<T, 8>::allocate(blocksCount);
			_first = _buffer;
			_last = _buffer + blocksCount;
			_free = nullptr;
		}

		~block_pool()
		{
			memory<T, 8>::free(_buffer);

			if(_next != nullptr)
				delete _next;
		}

		T * allocate()
		{
			if(_free != nullptr)
			{
				T * ptr = reinterpret_cast<T *>(_free);
				_free = *reinterpret_cast<void **>(_free);

				return ptr;
			}

			if(_first < _last)
			{
				T * ptr = reinterpret_cast<T *>(_first);
				_first = ptr + 1;
				return ptr;
			}

			return next_alloc();
		}

		void free(void * ptr)
		{
			*reinterpret_cast<void **>(ptr) = _free;
			_free = ptr;
		}

		static const size_t blockSize = sizeof(T);

	private:
		T * _buffer;
		void * _first;
		void * _last;
		void * _free;

		block_pool * _next = nullptr;

		T * next_alloc()
		{
			if(_next == nullptr)
				_next = new block_pool();

			return _next->allocate();
		}
	};

	/**
	 *	@brief
	 *	Safe and lock-free memory pool.
	 *	<blocksCount> MUST be a power of 2 and MUST NOT be greater than
	 *	0x10000 because of memory optimization of indices.
	 *
	 *	Each pool allocates <blocksCount> * sizeof(<T>) bytes.
	 *	If there is more memory needed, next pool is allocated automatically.
	 */
	template<class T, uint blocksCount>
	class block_pool<T, blocksCount, thread_safe_pool> : protected std::mutex
	{
		deny_copy(block_pool);

	public:
		block_pool()
		{
			_buffer = memory<T>::allocate(blocksCount);

			for(register ushort i = 0; i <= lastBlock; ++i)
				_freeList[i] = i;
		}

		~block_pool()
		{
			memory<T>::free(_buffer);

			if(_next != nullptr)
				delete _next;
		}

		T * allocate()
		{
			if(_count.load() <= 0)
				return next_alloc();

			int c = --_count;

			if(c <= 0)
			{
				++_count;
				return next_alloc();
			}

			volatile uint & place = _freeList[++_head % blocksCount];

			while(place == erased_block);

			T * ptr = _buffer + place;
			place = erased_block;

			return ptr;
		}

		void free(void * ptr)
		{
			T * _ptr = reinterpret_cast<T *>(ptr);

			if(_ptr < _buffer || _ptr > _buffer + lastElement)
			{
				if(_next)
					_next->free(_ptr);

				return;
			}

			_freeList[++_tail % blocksCount] = static_cast<uint>(_ptr - _buffer);
			++_count;
		}

		static const size_t blockSize = sizeof(T);

	private:
		static const ushort lastBlock = static_cast<ushort>(blocksCount - 1);
		static const uint lastElement = static_cast<uint>(lastBlock * blockSize);

		T * _buffer;
		uint _freeList[blocksCount];
		atomic<ushort> _count = static_cast<ushort>(blocksCount);
		atomic<ushort> _head = 0;
		atomic<ushort> _tail = 0;

		block_pool * _next = nullptr;

		T * next_alloc()
		{
			if(_next == nullptr)
			{
				lock();

				if(_next == nullptr)
					_next = new block_pool();

				unlock();
			}

			return _next->allocate();
		}
	};

	/**
	 *	@brief
	 *	Data pool without thread-safety.
	 *	<blocksCount> MUST be a power of 2 and MUST NOT be greater than
	 *	0x10000 because of memory optimization of indices.
	 *
	 *	Each pool allocates <blocksCount> * sizeof(<type>) bytes.
	 *	If there is more memory needed, next pool is allocated automatically.
	 *
	 *	Data isn't erasing while freeing, it is the difference from BlockPool
	 */
	template<class T, uint blocksCount = 0x1000, class ThreadPolicy = empty>
	class data_pool
	{
		deny_copy(data_pool);

		struct page
		{
			page(data_pool * pool, page * prev) : _pool(pool), _prev(prev)
			{
				_buffer = memory<T>::allocate(blocksCount);

				if(_prev != nullptr)
					_prev->_next = this;
			}

			~page()
			{
				if(_allocated > 0)
				{
					for(auto * ptr = _buffer; ptr < _buffer + _allocated; ++ptr)
						ptr->~T();
				}

				memory<T>::free(_buffer);

				if(_prev != nullptr)
					delete _prev;
			}

			data_pool * _pool;
			page * _prev;
			page * _next = nullptr;

			T * _buffer;
			uint _freeList[blocksCount];
			uint _free = erased_block;
			uint _acquired = 0;
			uint _allocated = 0;
		};

	public:
		data_pool()
		{
			_first = new page(this, nullptr);
			_current = _first;
		}

		~data_pool()
		{
			delete _current;
		}

		T * acquire()
		{
			page * p = _first;

			while(p != nullptr)
			{
				if(p->_free != erased_block)
				{
					++p->_acquired;
					++_acquired;
					T * ptr = p->_buffer + p->_free;
					p->_free = p->_freeList[p->_free];

					return ptr;
				}

				p = p->_next;
			}

			return nullptr;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		T * create(A &&... args)
		{
			if(_current->_allocated >= blocksCount)
			{
				++_pages;
				_current = new page(this, _current);
			}

			T * ptr = _current->_buffer + _current->_allocated;
			++_current->_acquired;
			++_current->_allocated;
			++_acquired;
			++_allocated;

			return new (ptr) T(forward<A>(args)...);
		}

		void free(T * ptr)
		{
			page * p = _current;

			while(p != nullptr)
			{
				if(ptr >= p->_buffer && ptr < p->_buffer + blocksCount)
				{
					--p->_acquired;
					--_acquired;
					/*
					if(p != _first && p->_acquired == 0 && _acquired < _pages * blocksCount * 2 / 3)
					{
						deletePage(p);
						return;
					}
					*/
					auto offset = static_cast<uint>(ptr - p->_buffer);
					p->_freeList[offset] = p->_free;
					p->_free = offset;
					return;
				}

				p = p->_prev;
			}
		}

		void freeAll()
		{
			page * p = _first;

			while(p != nullptr)
			{
				if(p->_allocated > 0)
				{
					p->_acquired = 0;
					p->_free = 0;
					clear(p->_freeList, p->_allocated);
				}

				p = p->_next;
			}

			_acquired = 0;
		}

		size_t count() const
		{
			return _allocated;
		}

		size_t acquired() const
		{
			return _acquired;
		}

		static const size_t blockSize = sizeof(T);

	private:
		static const ushort lastBlock = static_cast<ushort>(blocksCount - 1);
		static const uint lastElement = lastBlock * blockSize;

		void deletePage(page * p)
		{
			--_pages;
			_allocated -= p->_allocated;

			if(p == _current)
				_current = p->_prev;

			p->_prev->_next = p->_next;

			if(p->_next != nullptr)
				p->_next->_prev = p->_prev;

			p->_prev = nullptr;
			delete p;
		}

		size_t _acquired = 0;
		size_t _allocated = 0;
		uint   _pages = 1;
		page * _first = nullptr;
		page * _current = nullptr;
		
		static void clear(uint (& freeList)[blocksCount], uint count)
		{
			static const struct FreeList
			{
				FreeList()
				{
					for(uint i = 0; i < blocksCount - 1; ++i)
						list[i] = i + 1;

					list[blocksCount - 1] = erased_block;
				}

				uint list[blocksCount];
			} f;

			memory<uint>::move(freeList, f.list, count);
			freeList[count - 1] = erased_block;
		};
	};
}

//---------------------------------------------------------------------------
#endif
