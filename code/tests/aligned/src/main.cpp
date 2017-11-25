//---------------------------------------------------------------------------

#include <application/starter.h>

#include <math/vector.h>
#include <iostream>

//---------------------------------------------------------------------------

/**
 * @brief
 *
 * This benchmark allows to analyse overhead of `aligned` wrapper
 * Since all methods of `aligned` class are inlined, that overhead is pretty small
 *
 * @note
 * Vectors are created on the heap, otherwise there is no need for alignment
 */

namespace asd
{
	using namespace std::chrono;
	
	using vector = math::vector<float>;
	using aligned_vector = math::aligned_vec<float>;
	
	typedef high_resolution_clock hrc;
	
	time_point<hrc> last;
	long long t = 0;
	
	const int COUNT = 500;
	
	void create() {
		last = hrc::now();
		
		repeat(i, COUNT) {
			volatile vector * v = new vector();
			delete v;
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "plain vector time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, COUNT) {
			volatile aligned_vector p;
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "aligned vector time: " << t << std::endl;
	}
	
	void assign() {
		vector * v1 = new vector, * v2 = new vector;
		aligned_vector p1, p2;
		
		last = hrc::now();
		
		repeat(i, COUNT) {
			*v1 = *v2;
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "plain vector time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, COUNT) {
			p1 = p2;
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "aligned vector time: " << t << std::endl;
		
		delete v1;
		delete v2;
	}
	
	void multiply() {
		vector * v1 = new vector, * v2 = new vector;
		aligned_vector p1, p2;
		
		last = hrc::now();
		
		repeat(i, COUNT) {
			*v1 *= *v2;
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "plain vector time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, COUNT) {
			p1 *= p2;
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "aligned vector time: " << t << std::endl;
		
		delete v1;
		delete v2;
	}
	
	static entrance open([]() {
		std::cout << "create: " << std::endl;
		create();
		
		std::cout << "assign: " << std::endl;
		assign();
		
		std::cout << "multiply: " << std::endl;
		multiply();
		return 0;
	});
}

//---------------------------------------------------------------------------
