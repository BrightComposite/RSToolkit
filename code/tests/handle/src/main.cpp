//---------------------------------------------------------------------------

#include <application/starter.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	const int HANDLES_COUNT = 500;
	
	using namespace std::chrono;
	using hrc = high_resolution_clock;
	
	time_point<hrc> last;
	long long t = 0;
	
	void create() {
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			volatile boost::intrusive_ptr<object> h(new object, true);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "boost intrusive_ptr time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			volatile boost::movelib::unique_ptr<object> h(new object);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "boost unique_ptr time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			volatile handle<object> h(_);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "asd handle time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			volatile unique<object> h(_);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "asd unique time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			volatile std::shared_ptr<object> h(new object);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "std shared_ptr time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			volatile std::unique_ptr<object> h1(new object);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "std unique_ptr time: " << t << std::endl;
	}
	
	void assign() {
		auto asd_h = make::handle<object>();
		auto asd_u = make::unique<object>();
		auto bst_i = boost::intrusive_ptr<object>(new object);
		auto bst_u = boost::movelib::unique_ptr<object>(new object);
		auto std_s = std::make_shared<object>();
		auto std_u = std::make_unique<object>();
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			boost::intrusive_ptr<object> h = std::move(bst_i);
			bst_i = std::move(h);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "boost intrusive_ptr time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			boost::movelib::unique_ptr<object> h = std::move(bst_u);
			bst_u = std::move(h);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "boost unique_ptr time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			handle<object> h = std::move(asd_h);
			asd_h = std::move(h);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "asd handle time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			unique<object> h = std::move(asd_u);
			asd_u = std::move(h);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "asd unique time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			std::shared_ptr<object> h = std::move(std_s);
			std_s = std::move(h);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "std shared_ptr time: " << t << std::endl;
		
		last = hrc::now();
		
		repeat(i, HANDLES_COUNT) {
			std::unique_ptr<object> h = std::move(std_u);
			std_u = std::move(h);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		std::cout << "std unique_ptr time: " << t << std::endl;
	}
	
	static entrance open([]() {
		std::cout << "create: " << std::endl;
		create();
		
		std::cout << "assign: " << std::endl;
		assign();
	});
}

//---------------------------------------------------------------------------
