//---------------------------------------------------------------------------

#include <application/starter.h>
#include <space/spatial.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	using namespace std::chrono;
	using hrc = high_resolution_clock;
	
	time_point<hrc> last;
	long long t = 0;
	
	const int COUNT = 10000;
	
	static entrance main([]() {
		space::positioned s;
		space::vector v;
		space::vector x;
		
		last = hrc::now();
		
		repeat(i, COUNT) {
			s.move(x);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "spatial pos time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, COUNT) {
			v += x;
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "plain vector time: " << t << std::endl;
		
		return 0;
	});
}

//---------------------------------------------------------------------------
