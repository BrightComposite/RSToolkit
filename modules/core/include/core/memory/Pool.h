//---------------------------------------------------------------------------

#pragma once

#ifndef POOL_H
#define POOL_H

//---------------------------------------------------------------------------

#include "Memory.h"

#include <atomic>
#include <mutex>

//---------------------------------------------------------------------------

using std::atomic;

namespace asd
{
#define erased_block 0xffffffff

	struct ThreadSafePool {};

	/**
	 *	@brief
	 *	Basic memory pool without thread-safety.
	 *	<blocksCount> MUST be a power of 2 and MUST NOT be greater than
	 *	0x10000 because of memory optimization of indices.
	 *
	 *	Each pool allocates <blocksCount> * sizeof(<type>) bytes.
	 *	If there is more memory needed, next pool is allocated automatically.
	 */
	template<class T, uint blocksCount = 0x1000, class ThreadPolicy = Empty>
	class BlockPool
	{
		deny_copy(BlockPool);

	public:
		BlockPool()
		{
			_buffer = Memory<T, 8>::allocate(blocksCount);
			_first = _buffer;
			_last = _buffer + blocksCount;
			_free = nullptr;
		}

		~BlockPool()
		{
			Memory<T, 8>::free(_buffer);

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

		BlockPool * _next = nullptr;

		T * next_alloc()
		{
			if(_next == nullptr)
				_next = new BlockPool();

			return _next->allocate();
		}
	};

	/**
	 *	@brief
	 *	Safe and lock-free memory pool.
	 *	<blocksCount> MUST be a power of 2 and MUST NOT be greater than
	 *	0x10000 because of memory optimization of indices.
	 *
	 *	Each pool allocates <blocksCount> * sizeof(<type>) bytes.
	 *	If there is more memory needed, next pool is allocated automatically.
	 */
	template<class T, uint blocksCount>
	class BlockPool<T, blocksCount, ThreadSafePool> : protected std::mutex
	{
		deny_copy(BlockPool);

	public:
		BlockPool()
		{
			_buffer = Memory<T>::allocate(blocksCount);

			for(register ushort i = 0; i <= lastBlock; ++i)
				_freeList[i] = i;
		}

		~BlockPool()
		{
			Memory<T>::free(_buffer);

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

			uint & place = _freeList[++_head % blocksCount];

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

		static const size_t blockSize = sizeof(type);

	private:
		static const ushort lastBlock = static_cast<ushort>(blocksCount - 1);
		static const uint lastElement = lastBlock * blockSize;

		T * _buffer;
		uint _freeList[blocksCount];
		atomic<ushort> _count = blocksCount;
		atomic<ushort> _head = 0;
		atomic<ushort> _tail = 0;

		BlockPool * _next = nullptr;

		T * next_alloc()
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
	template<class T, uint blocksCount = 0x1000, class ThreadPolicy = Empty>
	class DataPool
	{
		deny_copy(DataPool);

		struct Page
		{
			Page(DataPool * pool, Page * prev) : _pool(pool), _prev(prev)
			{
				_buffer = Memory<T>::allocate(blocksCount);

				if(_prev != nullptr)
					_prev->_next = this;
			}

			~Page()
			{
				if(_allocated > 0)
				{
					for(auto * ptr = _buffer; ptr < _buffer + _allocated; ++ptr)
						ptr->~T();
				}

				Memory<T>::free(_buffer);

				if(_prev != nullptr)
					delete _prev;
			}

			DataPool * _pool;
			Page * _prev;
			Page * _next = nullptr;

			T * _buffer;
			uint _freeList[blocksCount];
			uint _free = erased_block;
			uint _acquired = 0;
			uint _allocated = 0;
		};

	public:
		DataPool()
		{
			_first = new Page(this, nullptr);
			_current = _first;
		}

		~DataPool()
		{
			delete _current;
		}

		T * acquire()
		{
			Page * p = _first;

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
				_current = new Page(this, _current);
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
			Page * p = _current;

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
			Page * p = _first;

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

		void deletePage(Page * p)
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
		Page * _first = nullptr;
		Page * _current = nullptr;
		
		static void clear(uint (& freeList)[blocksCount], uint count)
		{
			static const struct FreeList
			{
				FreeList()
				{
					for(int i = 0; i < blocksCount - 1; ++i)
						list[i] = i + 1;

					list[blocksCount - 1] = erased_block;
				}

				uint list[blocksCount];
			} f;

			Memory<uint>::move(freeList, f.list, count);
			freeList[count - 1] = erased_block;
		};
	};
}

//---------------------------------------------------------------------------
#endif
