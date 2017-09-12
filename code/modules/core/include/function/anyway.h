//---------------------------------------------------------------------------

#pragma once

#ifndef ANYWAY_H
#define ANYWAY_H

//---------------------------------------------------------------------------

#include <core/function/function.h>

//---------------------------------------------------------------------------

namespace asd
{
	class anyway
	{
	public:
		~anyway() {
			for(auto & f : _functions) {
				f();
			}
		}
		
		template<class F, class ... A, typename = decltype(declval<F>()(declval<A>()...))>
		void add(F f, A && ... args) {
			_functions.emplace_back(std::bind(f, forward<A>(args)...));
		}
	
	protected:
		std::vector<function<void()>> _functions;
	};
}

//---------------------------------------------------------------------------
#endif
