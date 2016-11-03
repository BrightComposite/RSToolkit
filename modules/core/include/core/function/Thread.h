//---------------------------------------------------------------------------

#pragma once

#ifndef THREAD_H
#define THREAD_H

//---------------------------------------------------------------------------

#include <thread>
#include <core/Handle.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::thread;
	namespace this_thread = std::this_thread;

	struct Thread : public Shared
	{
		template<class ... A, useif<can_construct<thread, A...>::value>>
		Thread(A &&... args) : instance(forward<A>(args)...) {}
		Thread(Thread && th) : instance(move(th.instance)) {}
		~Thread() { if(instance.joinable()) instance.join(); }

		Thread(const Thread &) = delete;
		Thread & operator = (const Thread &) = delete;

		Thread & operator = (Thread && th)
		{
			instance = move(th.instance);
			return *this;
		}

		thread instance;
	};
}

//---------------------------------------------------------------------------
#endif
