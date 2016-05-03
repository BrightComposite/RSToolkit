//---------------------------------------------------------------------------

#ifndef QUEUE_H
#define QUEUE_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <queue>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::deque;
	using std::queue;

	template<class T, class ... OwnerAttr>
	class Deque : public deque<Handle<T, OwnerAttr...>>
	{
	public:
		using deque<Handle<T, OwnerAttr...>>::deque;
	};

	template<class T, class ... OwnerAttr>
	class Queue : public queue<Handle<T, OwnerAttr...>>
	{
	public:
		using queue<Handle<T, OwnerAttr...>>::queue;
	};
}

//---------------------------------------------------------------------------
#endif
