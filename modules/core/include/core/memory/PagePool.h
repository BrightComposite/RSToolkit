//---------------------------------------------------------------------------

#pragma once

#ifndef PAGE_POOL_H
#define PAGE_POOL_H

//---------------------------------------------------------------------------

#include "Memory.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	template<size_t pageSize>
	class Page
	{
	public:
		Page(Page * next) : _ptr(malloc(pageSize)), _next(next) {}
		Page(const Page &) = delete;
		Page & operator = (const Page &) = delete;

		~Page()
		{
			free(_ptr);
		}

		void * _ptr;
		Page * _next;
	};

	template<size_t pageSize = 0x10000>
	class PagePool
	{
	public:
		typedef Page<pageSize> PageType;

		PagePool() : _page(nullptr) {}
		PagePool(const PagePool &) = delete;
		PagePool & operator = (const PagePool &) = delete;

		virtual ~PagePool()
		{
			PageType * next;

			for(PageType * page = _page; page != nullptr; page = next)
			{
				next = page->_next;
				delete page;
			}
		}

		void * allocatePage()
		{
			_page = new PageType(_page);
			return _page->_ptr;
		}

	private:
		PageType * _page;
	};
}

//---------------------------------------------------------------------------
#endif
