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
#include <boost/optional.hpp>

#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace asd
{
	class window;
	
	namespace gfx
	{
		class component
		{
			morph_origin(component)
		};
		
		template <class T, useif<is_base_of<component, T>::value>>
		morph_id_t component_id = morph_id<T>;
		
		class context
		{
			deny_copy(context);
			
		public:
			context() {}
			virtual ~context() {}
			
			template <class T>
			boost::optional<T &> component() const {
				auto i = _components.find(component_id<T>);
				return i != _components.end() ? static_cast<T &>(*i.value()) : boost::optional<T &>{};
			}
			
			virtual void flush() {}
		
		protected:
			template <class Gfx>
			friend
			class driver;
			
			template <class T>
			void register_component(unique<gfx::component> && p) {
				_components.insert_or_assign(component_id<T>, std::move(p));
			}
			
			map<morph_id_t, unique<gfx::component>> _components;
		};
		
		template <class Gfx>
		class driver_context : public context {};
		
		template <class Gfx>
		class driver
		{
			deny_copy(driver);
		
		public:
			using context_type = driver_context<Gfx>;
			using extender = function<unique<gfx::component>(context_type &)>;
			
			driver() {}
			
			template <class Interface, class Implementation, class ... A, useif<
				is_base_of<gfx::component, Interface>::value,
				is_base_of<Interface, Implementation>::value,
				can_construct<Implementation, context_type &, A...>::value
			>>
			void register_component(A && ... args) {
				auto ext = [=](context_type & ctx) {
					return make::unique<Implementation>(ctx, std::forward<A>(args)...);
				};
				
				for (auto & ctx : _contexts) {
					ctx->register_component<Interface>(ext(*ctx));
				}
			}
		
		private:
			array_list<unique<context_type>> _contexts;
			map<morph_id_t, extender> _extenders;
		};
	}
	
	using graphics = gfx::context;
	
	exception_class(graphics_exception, "Graphics exception");
	exception_subclass(context_creation_exception, graphics_exception, "Exception occurred while creating graphic context");
	exception_subclass(component_not_found_exception, graphics_exception, "Requested component was not found at given context");
	
	template <class T, useif<is_base_of<gfx::component, T>::value>>
	T & get(const gfx::context & ctx) {
		auto component = ctx.component<T>();
		
		if (component) {
			return component.get();
		}
		
		throw component_not_found_exception();
	}
}

//---------------------------------------------------------------------------
#endif
