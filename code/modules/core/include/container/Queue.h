//---------------------------------------------------------------------------

#pragma once

#ifndef QUEUE_H
#define QUEUE_H

//---------------------------------------------------------------------------

#include <core/handle.h>
#include <container/Container.h>

#include <queue>

//---------------------------------------------------------------------------

namespace asd
{
	using std::deque;
	using std::queue;

	template<class T, class ... Owner>
	class Deque : public deque<handle<T, Owner...>>
	{
	public:
		using deque<handle<T, Owner...>>::deque;
	};

	template<class T, class ... Owner>
	class Queue : public queue<handle<T, Owner...>>
	{
	public:
		using queue<handle<T, Owner...>>::queue;
	};
}

//---------------------------------------------------------------------------
#endif
