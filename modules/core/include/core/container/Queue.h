//---------------------------------------------------------------------------

#ifndef QUEUE_H
#define QUEUE_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <core/container/Container.h>

#include <queue>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::deque;
	using std::queue;

	template<class T, class ... Owner>
	class Deque : public deque<Handle<T, Owner...>>
	{
	public:
		using deque<Handle<T, Owner...>>::deque;
	};

	template<class T, class ... Owner>
	class Queue : public queue<Handle<T, Owner...>>
	{
	public:
		using queue<Handle<T, Owner...>>::queue;
	};
}

//---------------------------------------------------------------------------
#endif
