//---------------------------------------------------------------------------

#ifndef QUEUE_H
#define QUEUE_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <queue>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::deque;
	using std::queue;

	template<class T>
	class DequeObject : public Object, public deque<T>
	{
	public:
		using deque<T>::deque;
	};

	template<class T, class ... OwnerAttr>
	using HandleDeque = deque<Handle<T, OwnerAttr...>>;

	template<typename T, class ... OwnerAttr>
	using Deque = DequeObject<Handle<T, OwnerAttr...>>;

	template<class T>
	class QueueObject : public Object, public queue<T>
	{
	public:
		using queue<T>::queue;
	};

	template<class T, class ... OwnerAttr>
	using HandleQueue = queue<Handle<T, OwnerAttr...>>;

	template<typename T, class ... OwnerAttr>
	using Queue = QueueObject<Handle<T, OwnerAttr...>>;
}

//---------------------------------------------------------------------------
#endif
