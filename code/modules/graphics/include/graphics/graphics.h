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
#include <container/any_list.h>

#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace asd
{
	class window;
	
	namespace gfx
	{
		struct primitive
		{
			morph_origin(primitive);
		};

		class context
		{
		public:
			using entry_type = pair<morph_id_t, primitive>;
			
			virtual ~context() {}

			virtual void draw() = 0;

			template <class T>
			void operator <<(const T & value) {
				_list.push_back(pair<morph_id_t, T>{morph_id<T>, value});
			}
		
		protected:
			any_list _list;
		};
		
		template <class Gfx>
		class driver_context : public context
		{
			using entry_type = pair<morph_id_t, primitive>;
		
		public:
			driver_context(Gfx & driver) : driver(driver) {}
		
		protected:
			Gfx & driver;
		};
		
		template <class Gfx>
		class driver
		{
			using context_type = driver_context<Gfx>;
			using method_type  = function<void(context_type & ctx, const primitive & p)>;
			using draw_methods = array_list<method_type>;
		
		public:
			void call(context_type & context, morph_id_t type_id, const primitive & p) {
				if(type_id >= _methods.size() || _methods[type_id] == nullptr) {
					return;
				}
				
				_methods[type_id](context, p);
			}
			
			template <class Primitive, useif<is_morph_of<primitive, Primitive>::value>>
			bool has() const {
				return morph_id<Primitive> < _methods.size() && _methods[morph_id<Primitive>] != nullptr;
			}
			
			template <class Primitive, useif<is_morph_of<primitive, Primitive>::value>>
			void extend(void(* method)(context_type &, const Primitive &)) {
				if(morph_id<Primitive> >= _methods.size()) {
					_methods.resize(morph_id<Primitive> + 1);
				}
				
				_methods[morph_id<Primitive>] = [=](context_type & ctx, const primitive & p) {
					method(ctx, static_cast<const Primitive &>(p));
				};
			}
		
		private:
			draw_methods _methods;
		};
		
		struct mesh : primitive
		{
			morph_type(graphics, mesh);

			uint id;
		};
		
		struct texture : primitive
		{
			morph_type(graphics, texture);

			uint id;
		};
	}
	
	using graphics = gfx::context;
	
	exception_class(GraphicsException);
	exception_subclass(ContextCreationException, GraphicsException);
}

//---------------------------------------------------------------------------
#endif
