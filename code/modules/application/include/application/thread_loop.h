//---------------------------------------------------------------------------

#pragma once

#ifndef THREAD_LOOP_H
#define THREAD_LOOP_H

//---------------------------------------------------------------------------

#include <core/addition/singleton.h>
#include <container/ArrayList.h>
#include <core/function/Function.h>
#include <core/Exception.h>

#include <chrono>

//---------------------------------------------------------------------------

namespace asd
{
	using namespace std::literals;

	class thread_loop;

	template struct api(application) singleton<thread_loop, thread_local_model>;

	class thread_loop : public singleton<thread_loop, thread_local_model>
	{
		friend singleton<thread_loop, thread_local_model>;

		using Iteration = function<int()>;

	public:
		static void run()
		{
			auto & loop = instance();

			if(loop.active)
				throw Exception("thread_loop is already running!");

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
							
							if(it == list.end()) {
								loop.active = false;
								return;
							}
							
							break;
						
						case 2:
							loop.active = false;
							return;
						
						default:
							break;
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
		thread_loop() {}
		thread_loop(const thread_loop &) = delete;
		~thread_loop() {}

		thread_loop & operator = (const thread_loop &) = delete;

		array_list<Iteration> iterations;
		bool active = false;
	};
}

//---------------------------------------------------------------------------
#endif
