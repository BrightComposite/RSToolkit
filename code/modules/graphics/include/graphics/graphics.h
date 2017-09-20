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
			context & operator <<(const T & value) {
				_list.push_back(pair<morph_id_t, T>{morph_id<T>, value});
				return *this;
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
			
			template <class G, class Primitive, used_t>
			friend inline G & operator << (G & d, void(*method)(typename G::context_type &, const Primitive &));

			template <class G, class F, used_t>
			friend inline G & operator << (G & d, F functor);

			template <class G, class F, class Primitive, used_t>
			friend inline G & extend(G & d, F * functor, void (F::*method)(typename G::context_type &, const Primitive &));

			template <class G, class F, class Primitive, used_t>
			friend inline G & extend(G & d, F * functor, void (F::*method)(typename G::context_type &, const Primitive &) const);

		private:
			draw_methods _methods;
		};

		template<class Gfx, class T>
		struct is_extension_method : false_type {};

		template<class Gfx, class F, class Primitive>
		struct is_extension_method<Gfx, void (F::*)(typename Gfx::context_type &, const Primitive &)> : true_type {};

		template<class Gfx, class F, class Primitive>
		struct is_extension_method<Gfx, void (F::*)(typename Gfx::context_type &, const Primitive &) const> : true_type {};

		template <class Gfx, class Primitive, useif<is_morph_of<primitive, Primitive>::value>>
		inline Gfx & operator << (Gfx & d, void(* method)(typename Gfx::context_type &, const Primitive &)) {
			if(morph_id<Primitive> >= d._methods.size()) {
				d._methods.resize(morph_id<Primitive> + 1);
			}

			d._methods[morph_id<Primitive>] = [=](typename Gfx::context_type & ctx, const primitive & p) {
				method(ctx, static_cast<const Primitive &>(p));
			};

			return d;
		}

		template <class Gfx, class F, useif<is_extension_method<Gfx, decltype(&F::operator())>::value>>
		inline Gfx & operator << (Gfx & d, F functor) {
			return extend(d, &functor, &F::operator());
		}

		template <class Gfx, class F, class Primitive, useif<is_morph_of<primitive, Primitive>::value>>
		inline Gfx & extend(Gfx & d, F * functor, void (F::*method)(typename Gfx::context_type &, const Primitive &)) {
			if(morph_id<Primitive> >= d._methods.size()) {
				d._methods.resize(morph_id<Primitive> + 1);
			}

			d._methods[morph_id<Primitive>] = [=](typename Gfx::context_type & ctx, const primitive & p) {
				(functor->*method)(ctx, static_cast<const Primitive &>(p));
			};

			return d;
		}

		template <class Gfx, class F, class Primitive, useif<is_morph_of<primitive, Primitive>::value>>
		inline Gfx & extend(Gfx & d, F * functor, void (F::*method)(typename Gfx::context_type &, const Primitive &) const) {
			if(morph_id<Primitive> >= d._methods.size()) {
				d._methods.resize(morph_id<Primitive> + 1);
			}

			d._methods[morph_id<Primitive>] = [=](typename Gfx::context_type & ctx, const primitive & p) {
				(functor->*method)(ctx, static_cast<const Primitive &>(p));
			};

			return d;
		}
	}
	
	using graphics = gfx::context;
	
	exception_class(GraphicsException);
	exception_subclass(ContextCreationException, GraphicsException);
}

//---------------------------------------------------------------------------
#endif
