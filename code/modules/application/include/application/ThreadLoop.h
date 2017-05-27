//---------------------------------------------------------------------------

#pragma once

#ifndef THREAD_LOOP_H
#define THREAD_LOOP_H

//---------------------------------------------------------------------------

#include <core/addition/Singleton.h>
#include <container/ArrayList.h>
#include <core/function/Function.h>
#include <core/Exception.h>

#include <chrono>

//---------------------------------------------------------------------------

namespace asd
{
	using namespace std::literals;

	class ThreadLoop;

	template struct api(application) Singleton<ThreadLoop, ThreadLocalModel>;

	class ThreadLoop : public Singleton<ThreadLoop, ThreadLocalModel>
	{
		friend Singleton<ThreadLoop, ThreadLocalModel>;

		using Iteration = function<int()>;

	public:
		static void run()
		{
			auto & loop = instance();

			if(loop.active)
				throw Exception("ThreadLoop is already running!");

			loop.active = true;

			while(true)
			{
				auto & list = loop.iterations;

				if(list.empty())
				{
					loop.active = false;
					return;
				}

				for(auto it = list.begin(); it != list.end(); ++it)
				{
					int result = (*it)();

					switch(result)
					{
					case 1:
						it = list.erase(it);

						if(it == list.end())
						{
							loop.active = false;
							return;
						}

						break;

					case 2:
						loop.active = false;
						return;
					}

					if(!loop.active)
						return;
				}
			}
		}

		static bool isActive()
		{
			return instance().active;
		}

		static void stop()
		{
			instance().active = false;
		}

		// Conditional iterations
		template<class F, useif<is_same<decltype(declval<F>()()), int>::value>>
		static void add(F & iteration)
		{
			instance().iterations.emplace_back(iteration);
		}

		template<class F, useif<is_same<decltype(declval<F>()()), int>::value>>
		static void add(F && iteration)
		{
			instance().iterations.emplace_back(forward<F>(iteration));
		}

		// Clean iterations
		template<class F, skipif<is_same<decltype(declval<F>()()), int>::value>>
		static void add(F & iteration)
		{
			instance().iterations.emplace_back([&iteration]() mutable {
				iteration();
				return 0;
			});
		}

		template<class F, skipif<is_same<decltype(declval<F>()()), int>::value>>
		static void add(F && iteration)
		{
			instance().iterations.emplace_back([iteration]() mutable {
				iteration();
				return 0;
			});
		}

	protected:
		ThreadLoop() {}
		ThreadLoop(const ThreadLoop &) = delete;
		~ThreadLoop() {}

		ThreadLoop & operator = (const ThreadLoop &) = delete;

		array_list<Iteration> iterations;
		bool active = false;
	};
}

//---------------------------------------------------------------------------
#endif
