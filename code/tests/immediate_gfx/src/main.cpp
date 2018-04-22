
//---------------------------------------------------------------------------

#include <container/map.h>
#include <container/any_list.h>
#include <meta/class_id.hpp>
#include <application/starter.h>

#include <benchmark>

//---------------------------------------------------------------------------

// #define GFX_METHODS_USE_MAP

namespace asd
{
	/**
	 * Basic conceptual classes
	 */
	
	struct gfx_primitive
	{
		origin_class(gfx_primitive);
	};

	struct primitive {};

	struct basic_primitive_key { origin_class(basic_primitive_key); };

	template<class T>
	struct primitive_key : basic_primitive_key { morph_type(primitive_key<T>); };

	template<class T>
	class_id_t primitive_id = class_id<primitive_key<T>>;

	class gfx_context
	{
		using entry_type = pair<class_id_t, gfx_primitive>;
	
	public:
		template <class T>
		void operator <<(const T & value) {
			draw(class_id<T>, value);
		}
		
		virtual void draw(class_id_t, const gfx_primitive &) = 0;
	};
	
	template <class Gfx>
	class driver_context : public gfx_context
	{
		using entry_type = pair<class_id_t, gfx_primitive>;
	
	public:
		driver_context(Gfx & device) : driver(device) {}
		
		void draw(class_id_t id, const gfx_primitive & p) override {
			driver.call(*this, id, p);
		}
		
		Gfx & driver;
	};
	
	template <class Gfx>
	class api(immediate_gfx_test) gfx_driver
	{
		using context_type = driver_context<Gfx>;
		
		using method_type = function<void(context_type & ctx, const gfx_primitive & p)>;

#ifdef GFX_METHODS_USE_MAP
		using draw_methods = map<class_id_t, method_type>;
#else
		using draw_methods = array_list<method_type>;
#endif
	
	public:
		unique<context_type> create_context() {
			return new driver_context<Gfx>(static_cast<Gfx &>(*this));
		}
		
		void call(context_type & context, class_id_t type_id, const gfx_primitive & p) {
#ifdef GFX_METHODS_USE_MAP
			auto it = _methods.find(type_id);
			
			if(it == _methods.end()) {
				return;
			}
			
			it->second(context, p);
#else
			if(type_id >= _methods.size() || _methods[type_id] == nullptr) {
				return;
			}
			
			_methods[type_id](context, p);
#endif
		}
		
		template <class Primitive, useif<is_base_of<gfx_primitive, Primitive>::value>>
		void extend(void(* method)(driver_context<Gfx> &, const Primitive &)) {
#ifndef GFX_METHODS_USE_MAP
			if(class_id<Primitive> >= _methods.size()) {
				_methods.resize(class_id<Primitive> + 1);
			}
#endif
			
			_methods[class_id<Primitive>] = [=](driver_context<Gfx> & ctx, const gfx_primitive & p) {
				method(ctx, static_cast<const Primitive &>(p));
			};
		}
	
	private:
		draw_methods _methods;
	};
	
	/**
	 * Virtual alternative
	 */
	
	class virtual_context
	{
		using draw_call = function<void()>;
	
	public:
		virtual void increment(int value) = 0;
		virtual void decrement(int value) = 0;
	};
	
	/**
	 *	Drawer alternative
	 */

	struct Drawer {
		virtual ~Drawer() {}
	};

	class IncDec : public Drawer
	{
	public:
		function<void(int)> increment;
		function<void(int)> decrement;
	};

	class VirtualIncDec : public Drawer
	{
	public:
		virtual void increment(int) = 0;
		virtual void decrement(int) = 0;
	};

	class drawer_context
	{
	public:
		template<class Drawer>
		Drawer * drawer() {
			return static_cast<Drawer *>(request(primitive_id<Drawer>));
		}

	protected:
		virtual Drawer * request(class_id_t id) = 0;
	};

	/**
	 * Tested classes
	 */
	
	class Basic
	{
	public:
		int counter = 0;
	};
	
	class real_context : public virtual_context, public Basic
	{
	public:
		virtual void increment(int value) {
			counter += value;
		}
		
		virtual void decrement(int value) {
			counter -= value;
		}
	};

	class my_context : public drawer_context, public Basic
	{
	public:
		template<class Drawer, class Impl>
		void register_drawer() {
			if(primitive_id<Drawer> >= drawers.size()) {
				drawers.resize(primitive_id<Drawer> + 1);
			}

			drawers[primitive_id<Drawer>] = make::unique<Impl>(*this);
		}

	protected:
		virtual Drawer * request(class_id_t id) override {
			return id < drawers.size() ? drawers[id].get() : nullptr;
		}

	private:
		array_list<unique<Drawer>> drawers;
	};

	class Gfx : public gfx_driver<Gfx>, public Basic {};
	
	/**
	 * Declare two "primitives"
	 */
	
	struct Increment : gfx_primitive
	{
		morph_type(Increment);

		Increment(int value) : value(value) {}
		
		int value;
	};
	
	struct Decrement : gfx_primitive
	{
		morph_type(Decrement);

		Decrement(int value) : value(value) {}
		
		int value;
	};
	
	/**
	 * Declare extensions
	 */
	
	void increment(driver_context<Gfx> & obj, const Increment & p) {
		obj.driver.counter += p.value;
	}
	
	void decrement(driver_context<Gfx> & obj, const Decrement & p) {
		obj.driver.counter -= p.value;
	}

	struct MyIncDec : IncDec
	{
		MyIncDec(my_context & ctx) : ctx(ctx) {
			increment = std::bind(&MyIncDec::inc, this, std::placeholders::_1);
			decrement = std::bind(&MyIncDec::dec, this, std::placeholders::_1);
		}

		void inc(int val) {
			ctx.counter += val;
		}

		void dec(int val) {
			ctx.counter -= val;
		}

		my_context & ctx;
	};

	struct MyVirtualIncDec : VirtualIncDec
	{
		MyVirtualIncDec(my_context & ctx) : ctx(ctx) {}

		void increment(int val) override {
			ctx.counter += val;
		}

		void decrement(int val) override {
			ctx.counter -= val;
		}

		my_context & ctx;
	};

	/**
	 * Benchmark entry point
	 */
	
	static entrance open([]() {
		const int DRAW_CALLS = 1000;
		const int TRIES_COUNT = 100;
		
		Gfx driver;
		driver.extend(increment);
		driver.extend(decrement);
		
		// create real implementations
		auto ctx = driver.create_context();
		unique<real_context> r = new real_context;
		my_context dc;
		dc.register_drawer<IncDec, MyIncDec>();
		my_context vdc;
		vdc.register_drawer<VirtualIncDec, MyVirtualIncDec>();
		
		// cast to basic classes
		gfx_context & e = *ctx;
		virtual_context & v = *r;
		drawer_context & d = dc;
		drawer_context & vd = vdc;
		
		benchmark("extensible gfx", TRIES_COUNT) << [&]() {
			for(int i = 0; i < DRAW_CALLS; ++i) {
				e << Increment {29};
				e << Decrement {12};
			}
		};
		
		std::cout << ctx->driver.counter << std::endl;
		
		benchmark("virtual gfx", TRIES_COUNT) << [&]() {
			for(int i = 0; i < DRAW_CALLS; ++i) {
				v.increment(29);
				v.decrement(12);
			}
		};
		
		std::cout << r->counter << std::endl;

		benchmark("drawer gfx", TRIES_COUNT) << [&]() {
			auto drawer = d.drawer<IncDec>();
			
			if (drawer) {
				for(int i = 0; i < DRAW_CALLS; ++i) {
					drawer->increment(29);
					drawer->decrement(12);
				}
			}
		};

		std::cout << dc.counter << std::endl;

		benchmark("virtual drawer gfx: request every call", TRIES_COUNT) << [&]() {
			for(int i = 0; i < DRAW_CALLS; ++i) {
				auto drawer = vd.drawer<VirtualIncDec>();

				if (drawer) {
					drawer->increment(29);
					drawer->decrement(12);
				}
			}
		};

		std::cout << vdc.counter << std::endl;

		vdc.counter = 0;

		benchmark("virtual drawer gfx: request once", TRIES_COUNT) << [&]() {
			auto drawer = vd.drawer<VirtualIncDec>();

			if (drawer) {
				for(int i = 0; i < DRAW_CALLS; ++i) {
					drawer->increment(29);
					drawer->decrement(12);
				}
			}
		};

		std::cout << vdc.counter << std::endl;
	});
}

//---------------------------------------------------------------------------
