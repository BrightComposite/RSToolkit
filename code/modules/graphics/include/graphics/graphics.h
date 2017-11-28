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
		class interface
		{
			morph_origin(interface)
		};
		
		template <class T, useif<is_base_of<interface, T>::value>>
		morph_id_t interface_id = morph_id<T>;
		
		class context
		{
		public:
			virtual ~context() {}
			
			template <class T>
			boost::optional<T> interface() const {
				auto i = _interfaces.find(interface_id<T>);
				return i != _interfaces.end() ? static_cast<T &>(*i.value()) : boost::optional<T>{};
			}
			
			virtual void flush() {}
		
		protected:
			template <class Gfx>
			friend
			class driver;
			
			template <class T>
			void register_interface(unique<gfx::interface> && p) {
				_interfaces.insert_or_assign(interface_id<T>, std::move(p));
			}
			
			map<morph_id_t, unique<gfx::interface>> _interfaces;
		};
		
		template <class Gfx>
		class driver_context : public context {};
		
		template <class Gfx>
		class driver
		{
		public:
			using context_type = driver_context<Gfx>;
			using extender = function<unique<gfx::interface>(context_type &)>;
			
			template <class Interface, class Implementation, class ... A, useif<
				is_base_of<gfx::interface, Interface>::value,
				is_base_of<Interface, Implementation>::value,
				can_construct<Implementation, context_type &, A...>::value
			>>
			void register_interface(A && ... args) {
				auto ext = [=](context_type & ctx) {
					return make::unique<Implementation>(ctx, std::forward<A>(args)...);
				};
				
				for (auto & ctx : _contexts) {
					ctx->register_interface<Interface>(ext(*ctx));
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
	exception_subclass(interface_not_found_exception, graphics_exception, "Requested interface was not found at given context");
	
	template <class T, useif<is_base_of<gfx::interface, T>::value>>
	T & get(const gfx::context & ctx) {
		try {
			return ctx.interface<T>().value();
		} catch (const boost::bad_optional_access &) {
			throw interface_not_found_exception();
		}
	}
}

//---------------------------------------------------------------------------
#endif
