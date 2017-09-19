//---------------------------------------------------------------------------

#pragma once

#ifndef SHAREABLE_H
#define SHAREABLE_H

//---------------------------------------------------------------------------

#include <core/memory/allocator/default_alloc.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<class T>
	struct pointer_deleter
	{
		static void free(const T * ptr) {
			delete ptr;
		}
		
		void operator ()(const T * ptr) {
			delete ptr;
		}
	};
	
	//---------------------------------------------------------------------------

//#define ATOMIC_REFERENCES

#ifdef ATOMIC_REFERENCES
	using ref_counter_t = atomic<size_t>;
#else
	using ref_counter_t = size_t;
#endif
	
	/**
	 *  @brief
	 *  The shareable class is used to store the reference counter which can be
	 *	accessed by the handle class.
	 *
	 *	A shareable object is an object of a class T which inherits the shareable
	 *	class. Such thing allows to create handles to shareables. 
	 *	Create shareable classes when you know that objects of these classes will
	 *	be handled.
	 *
	 *	The template parameter T is used to ensure that `delete` will call the 
	 *	destructor of the class T, which may be virtual
	 */
	template<class T>
	struct shareable : public default_alloc
	{
		template<class>
		friend class handle;
		
		friend forceinline void intrusive_ptr_add_ref(const T * s) {
			++s->_refs;
		}
		
		friend forceinline void intrusive_ptr_release(const T * s) {
			if(--s->_refs == 0) {
				delete s;
			}
		}
	
	protected:
		mutable ref_counter_t _refs = 1;
	};

	template<class T>
	struct is_shareable : is_base_of<shareable<T>, T> {};
}

//---------------------------------------------------------------------------
#endif
