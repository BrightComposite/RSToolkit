/**
 * @brief
 * The main purpose of this benchmark is to show that batch draw calls can be
 * effectively implemented using a heterogeneous list of drawing primitives
 * and something like open methods, rather than a list of bound functions.
 *
 * Also, this approach allows you to expand the capabilities of classes
 * responsible for drawing primitives without need to wrap functionality
 * (you just receive a raw context and primitive to draw)
 */

//---------------------------------------------------------------------------

#include <container/map.h>
#include <container/any_list.h>
#include <meta/class_id.hpp>
#include <application/starter.h>

#include <benchmark>

//---------------------------------------------------------------------------

namespace asd
{
	/**
	 * Basic conceptual classes
	 */
	
	struct gfx_primitive
	{
		origin_class(gfx_primitive);
	};
	
	create_morph_pool(batch_gfx_test, gfx_primitive);
	
	class gfx_context
	{
		using entry_type = pair<class_id_t, gfx_primitive>;
	
	public:
		template <class T>
		void operator <<(const T & value) {
			_list.push_back(pair<class_id_t, T>{class_id<T>, value});
		}
	
	protected:
		any_list _list;
	};
	
	template <class Gfx>
	class driver_context : public gfx_context
	{
		using entry_type = pair<class_id_t, gfx_primitive>;
	
	public:
		driver_context(Gfx & device) : driver(device) {}
		
		void draw() {
			for(size_t i = 0; i < _list.size(); ++i) {
				auto & entry = _list.get_by_offset<entry_type>(_list.offset(i));
				driver.call(*this, entry.first, entry.second);
			}
			
			_list.clear();
		}
		
		Gfx & driver;
	};
	
	template <class Gfx>
	class api(batch_gfx_test) gfx_driver
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
		
		template <class F>
		void batch(const F & functor) {
			_list.push_back(functor);
		}
		
		void draw() {
			for(auto & entry : _list) {
				entry();
			}
			
			_list.clear();
		}
	
	protected:
		array_list<draw_call> _list;
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
			batch([=]() {
				counter += value;
			});
		}
		
		virtual void decrement(int value) {
			batch([=]() {
				counter -= value;
			});
		}
	};
	
	class Gfx : public gfx_driver<Gfx>, public Basic {};
	
	/**
	 * Declare two "primitives"
	 */
	
	struct Increment : gfx_primitive
	{
		Increment(int value) : value(value) {}
		
		int value;
	};
	
	struct Decrement : gfx_primitive
	{
		Decrement(int value) : value(value) {}
		
		int value;
	};
	
	create_morph_type(batch_gfx_test, Increment);
	
	create_morph_type(batch_gfx_test, Decrement);
	
	/**
	 * Declare extensions
	 */
	
	void increment(driver_context<Gfx> & obj, const Increment & p) {
		obj.driver.counter += p.value;
	}
	
	void decrement(driver_context<Gfx> & obj, const Decrement & p) {
		obj.driver.counter -= p.value;
	}
	
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
		
		// cast to basic classes
		gfx_context & e = *ctx;
		virtual_context & v = *r;
		
		benchmark("extensible gfx", TRIES_COUNT) << [&]() {
			for(int i = 0; i < DRAW_CALLS; ++i) {
				e << Increment {29};
				e << Decrement {12};
				ctx->draw();
			}
		};
		
		std::cout << ctx->driver.counter << std::endl;
		
		benchmark("virtual gfx", TRIES_COUNT) << [&]() {
			for(int i = 0; i < DRAW_CALLS; ++i) {
				v.increment(29);
				v.decrement(12);
				r->draw();
			}
		};
		
		std::cout << r->counter << std::endl;
	});
}

//---------------------------------------------------------------------------
