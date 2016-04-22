//---------------------------------------------------------------------------

#ifndef THREAD_H
#define THREAD_H

//---------------------------------------------------------------------------

#include <thread>
#include <core/Handle.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::thread;

	class Thread : public Shared
	{
	public:
		template<class ... A, useif(can_construct(thread, A...))>
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
