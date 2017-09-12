//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHICS_H
#define GRAPHICS_H

//---------------------------------------------------------------------------

#include <meta/useif.h>
#include <core/handle.h>
#include <morph/morph.h>
#include <container/map.h>
#include <container/array_list.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace gfx
	{
		template <class G>
		void gfx_draw(G & obj, const empty & p);
		
		struct primitive
		{
			morph_origin(primitive);
		};
	}
	
	create_morph_pool(graphics, gfx::primitive);
	
	namespace gfx
	{
		class context
		{
			using entry_type = pair<morph_id_t, primitive>;
		
		public:
			template <class T>
			void operator <<(const T & value) {
				draw(morph_id<T>, value);
			}
			
		protected:
			virtual void draw(morph_id_t, const primitive &) = 0;
		};
		
		template <class Gfx>
		class driver_context : public context
		{
			using entry_type = pair<morph_id_t, primitive>;
		
		public:
			driver_context(Gfx & device) : driver(device) {}
		
		protected:
			void draw(morph_id_t id, const primitive & p) override {
				driver.call(*this, id, p);
			}
			
			Gfx & driver;
		};
		
		template <class Gfx>
		class api(graphics) driver
		{
			using context_type = driver_context<Gfx>;
			
			template <class Primitive>
			static void draw(context_type & ctx, const primitive & p) {
				gfx_draw(ctx, static_cast<const Primitive &>(p));
			}
			
			using method_type = void (*)(context_type & ctx, const primitive & p);

#ifdef GFX_METHODS_USE_MAP
			using draw_methods = map<morph_id_t, method_type>;
#else
			using draw_methods = array_list<method_type>;
#endif
		
		public:
			unique<context_type> create_context() {
				return new driver_context<Gfx>(static_cast<Gfx &>(*this));
			}
			
			void call(context_type & context, morph_id_t type_id, const primitive & p) {
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
			
			template <class Primitive, useif<is_morph_of<primitive, Primitive>::value>>
			void extend() {
#ifndef GFX_METHODS_USE_MAP
				if(morph_id<Primitive> >= _methods.size()) {
					_methods.resize(morph_id<Primitive> + 1);
				}
#endif
				
				_methods[morph_id<Primitive>] = driver::draw<Primitive>;
			}
		
		private:
			draw_methods _methods;
		};
	}
}

//---------------------------------------------------------------------------
#endif
