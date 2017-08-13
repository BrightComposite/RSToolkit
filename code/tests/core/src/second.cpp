//---------------------------------------------------------------------------

#include <application/application.h>
#include <iostream>
#include <chrono>
#include <conio.h>

#include <thread>
#include <vld.h>

//---------------------------------------------------------------------------

namespace asd
{
	using std::cout;
	using std::endl;

	using std::chrono::high_resolution_clock;
	using std::chrono::time_point;
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;

	typedef high_resolution_clock hrc;


	static volatile Entrance second([]()
	{
		return 0;
	}, 1);

}

//---------------------------------------------------------------------------
