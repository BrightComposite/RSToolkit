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
		template<class Gfx>
		struct modifier;

		struct primitive {};
		
		struct basic_primitive_key { morph_origin(basic_primitive_key); };

		struct basic_modifier_key { morph_origin(basic_modifier_key); };

		template<class T>
		struct primitive_key : basic_primitive_key { morph_type(primitive_key<T>); };

		template<class T>
		struct modifier_key : basic_modifier_key { morph_type(modifier_key<T>); };

		template<class T>
		morph_id_t primitive_id = morph_id<primitive_key<T>>;

		template<class T>
		morph_id_t modifier_id = morph_id<modifier_key<T>>;

		class context
		{
		public:
			using entry_type = pair<morph_id_t, const primitive *>;
			
			virtual ~context() {}

			virtual void draw() = 0;

			template <class T>
			context & operator <<(const T & value) {
				_list.push_back(entry_type{primitive_id<T>, &value});
				return *this;
			}

			template <class T>
			context & operator << (T && value) = delete;

		protected:
			array_list<entry_type> _list;
		};
		
		template <class Gfx>
		class driver_context : public context {};

		template<class Gfx>
		struct modifier
		{
			virtual ~modifier() {}
			virtual void apply(driver_context<Gfx> & ctx) const = 0;
		};

		template <class Gfx>
		class driver
		{
		public:
			using context_type = driver_context<Gfx>;
			using method_type  = function<void(context_type & ctx, const primitive * p)>;
			using modifier_generator = function<void(context_type &)>;
			using draw_methods = array_list<method_type>;

			void call(context_type & context, morph_id_t type_id, const primitive * p) {
				if(type_id >= _methods.size() || _methods[type_id] == nullptr) {
					return;
				}
				
				_methods[type_id](context, p);
			}
			
			template <class Primitive, useif<is_morph_of<primitive, Primitive>::value>>
			bool has() const {
				return morph_id<Primitive> < _methods.size() && _methods[morph_id<Primitive>] != nullptr;
			}

			void set_modifiers(context_type & context) {
				for(auto & mod : _modifiers) {
					mod(context);
				}
			}

			template<class T>
			struct is_extension_method : false_type {};

			template<class F, class Primitive>
			struct is_extension_method<void (F::*)(context_type &, const Primitive &)> : true_type {};

			template<class F, class Primitive>
			struct is_extension_method<void (F::*)(context_type &, const Primitive &) const> : true_type {};

			template<class T>
			struct is_modification : false_type {};

			template<class F, class Modifier>
			struct is_modification<unique<Modifier> (F::*)(context_type &)> : true_type {};

			template<class F, class Modifier>
			struct is_modification<unique<Modifier> (F::*)(context_type &) const> : true_type {};

			template <class Primitive>
			driver & method(void(*method)(context_type &, const Primitive &)) {
				if(primitive_id<Primitive> >= _methods.size()) {
					_methods.resize(primitive_id<Primitive> + 1);
				}

				_methods[primitive_id<Primitive>] = [=](context_type & ctx, const primitive * p) {
					method(ctx, *static_cast<const Primitive *>(p));
				};

				return *this;
			}

			template <class F, useif<is_extension_method<decltype(&F::operator())>::value>>
			driver & method(F functor) {
				return method(&functor, &F::operator());
			}

			template <class F, class Primitive>
			driver & method(F * functor, void (F::*method)(context_type &, const Primitive &)) {
				if(primitive_id<Primitive> >= _methods.size()) {
					_methods.resize(primitive_id<Primitive> + 1);
				}

				_methods[primitive_id<Primitive>] = [=](context_type & ctx, const primitive * p) {
					(functor->*method)(ctx, *static_cast<const Primitive *>(p));
				};

				return *this;
			}

			template <class G, class F, class Primitive>
			driver & method(F * functor, void (F::*method)(context_type &, const Primitive &) const) {
				if(primitive_id<Primitive> >= _methods.size()) {
					_methods.resize(primitive_id<Primitive> + 1);
				}

				_methods[primitive_id<Primitive>] = [=](context_type & ctx, const primitive * p) {
					(functor->*method)(ctx, *static_cast<const Primitive *>(p));
				};

				return *this;
			}

			template<class T, class Modifier>
			void modifier(unique<Modifier> (*generator)(context_type &)) {
				_modifiers.push_back([=](context_type & ctx) {
					ctx.extend(modifier_id<T>, generator(ctx));
				});
			}

			template<class T, class F, useif<is_modification<decltype(&F::operator())>::value>>
			void modifier(F generator) {
				modifier<T>(&generator, &F::operator());
			}

			template <class T, class F, class Modifier>
			void modifier(F * f, unique<Modifier> (F::*method)(context_type &)) {
				_modifiers.push_back([=](context_type & ctx) {
					ctx.extend(modifier_id<T>, (f->*method)(ctx));
				});
			}

			template <class T, class F, class Modifier>
			void modifier(F * f, unique<Modifier> (F::*method)(context_type &) const) {
				_modifiers.push_back([=](context_type & ctx) {
					ctx.extend(modifier_id<T>, (f->*method)(ctx));
				});
			}

		protected:
			draw_methods _methods;
			array_list<modifier_generator> _modifiers;
		};
	}
	
	using graphics = gfx::context;
	
	exception_class(GraphicsException);
	exception_subclass(ContextCreationException, GraphicsException);
}

//---------------------------------------------------------------------------
#endif
