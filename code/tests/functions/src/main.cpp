//---------------------------------------------------------------------------

#include <application/starter.h>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <map>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <benchmark>

//---------------------------------------------------------------------------

namespace asd
{
	struct GenericContext
	{
		virtual ~GenericContext() {}
	};
	
	struct Context : GenericContext
	{
		virtual ~Context() {}
		
		forceinline void call() {
			++counter;
		}
		
		int counter = 0;
	};
	
	forceinline void callContext(Context & c) {
		c.call();
	}
	
	forceinline void callGenericContext(GenericContext & c) {
		callContext(static_cast<Context &>(c));
	}
	
	function_checker(can_call_context, callContext);
	
	class GenericCaller
	{
	public:
		template <class C, useif<can_call_context<C &>::value>>
		forceinline void operator ()(C & c) {
			callContext(c);
		}
		
		template <class C, skipif<can_call_context<C &>::value>>
		forceinline void operator ()(C & c) {
		
		}
	};
	
	class VirtualCaller
	{
	public:
		virtual ~VirtualCaller() {}
		
		virtual void operator ()(GenericContext & c) {}
	};
	
	class RealCaller : public VirtualCaller
	{
	public:
		RealCaller() {
		}
		
		virtual ~RealCaller() {}
		
		virtual void operator ()(GenericContext & c) override {
			callGenericContext(c);
		}
	};
	
	namespace asd
	{
		static entrance open([]() {
			VirtualCaller * vc = new RealCaller;
			GenericCaller * gc = new GenericCaller;
			
			const int N = 1000;
			
			using namespace std::chrono;
			
			Context c;
			
			c.counter = 0;
			
			benchmark("virtual caller") << [&]() {
				for(int i = 0; i < N; ++i) {
					vc->operator ()(c);
				}
			};
			
			std::cout << "result: " << c.counter << std::endl;
			
			c.counter = 0;
			
			benchmark("generic caller") << [&]() {
				for(int i = 0; i < N; ++i) {
					gc->operator ()(c);
				}
			};
			
			std::cout << "result: " << c.counter << std::endl;
			
			c.counter = 0;
			auto mgf = wrap_method(gc, static_cast<void (GenericCaller::*)(Context &)>(&GenericCaller::operator ()));
			
			benchmark("generic caller with method wrapper") << [&]() {
				for(int i = 0; i < N; ++i) {
					mgf(c);
				}
			};
			
			std::cout << "result: " << c.counter << std::endl;
			
			c.counter = 0;
			function<void(Context &)> af = *gc;
			
			benchmark("generic caller with asd::function") << [&]() {
				for(int i = 0; i < N; ++i) {
					af(c);
				}
			};
			
			std::cout << "result: " << c.counter << std::endl;
			
			c.counter = 0;
			auto gf = boost::bind(static_cast<void (GenericCaller::*)(Context &)>(&GenericCaller::operator ()), gc, ::_1);
			
			benchmark("generic caller with raw boost bind") << [&]() {
				for(int i = 0; i < N; ++i) {
					gf(c);
				}
			};
			
			std::cout << "result: " << c.counter << std::endl;
			
			c.counter = 0;
			boost::function<void(Context &)> bgf = boost::bind(static_cast<void (GenericCaller::*)(Context &)>(&GenericCaller::operator ()), gc, ::_1);
			
			benchmark("generic caller with boost::function") << [&]() {
				for(int i = 0; i < N; ++i) {
					bgf(c);
				}
			};
			
			std::cout << "result: " << c.counter << std::endl;
			
			c.counter = 0;
			auto rgf = std::bind(static_cast<void (GenericCaller::*)(Context &)>(&GenericCaller::operator ()), gc, std::placeholders::_1);
			
			benchmark("generic caller with raw std bind") << [&]() {
				for(int i = 0; i < N; ++i) {
					rgf(c);
				}
			};
			
			std::cout << "result: " << c.counter << std::endl;
			
			c.counter = 0;
			std::function<void(Context &)> sgf = std::bind(static_cast<void (GenericCaller::*)(Context &)>(&GenericCaller::operator ()), gc, std::placeholders::_1);
			
			benchmark("generic caller with std::function") << [&]() {
				for(int i = 0; i < N; ++i) {
					sgf(c);
				}
			};
			
			std::cout << "result: " << c.counter << std::endl;
			
			delete gc;
			delete vc;
			return 0;
		});
	}
}

//---------------------------------------------------------------------------
