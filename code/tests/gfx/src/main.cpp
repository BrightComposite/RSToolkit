//---------------------------------------------------------------------------

#include <application/starter.h>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <map>

#include <boost/function.hpp>
#include <boost/bind.hpp>

//---------------------------------------------------------------------------

namespace asd {
	static entrance open([]() {
		
		return 0;
	});
}

//---------------------------------------------------------------------------
