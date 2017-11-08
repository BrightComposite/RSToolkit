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
		class extension {
			morph_origin(extension)
		};
		
		template <class T, useif<is_base_of<extension, T>::value>>
		morph_id_t extension_id = morph_id<T>;
		
		class context
		{
		public:
			virtual ~context() {}

			template <class T>
			boost::optional<T> extension() const {
				auto i = _extensions.find(extension_id<T>);
				return i != _extensions.end() ? static_cast<T &>(*i.value()) : {};
			}
			
			virtual void flush() {}

		protected:
			template <class Gfx>
			friend class driver;

			template<class T>
			void register_extension(unique<gfx::extension> && p) {
				_extensions.insert_or_assign(extension_id<T>, std::move(p));
			}

			map<morph_id_t, unique<gfx::extension>> _extensions;
		};
		
		template <class Gfx>
		class driver_context : public context {};

		template <class Gfx>
		class driver
		{
		public:
			using context_type = driver_context<Gfx>;
			using extender = function<unique<gfx::extension>(context_type &)>;

			template<class Extension, class Implementation, class ... A, useif<
				is_base_of<gfx::extension, Extension>::value,
				is_base_of<Extension, Implementation>::value,
				can_construct<Implementation, context_type &, A...>::value
			>>
			void register_extension(A && ... args) {
				auto ext = [args {std::move(args)}...](context_type & ctx) {
					return make::unique<Implementation>(ctx, forward<A>(args)...);
				};

				for(auto & ctx : _contexts) {
					ctx->register_extension<Extension>(ext(*ctx));
				}
			}

		private:
			array_list<unique<context_type>> _contexts;
			map<morph_id_t, extender> _extenders;
		};
	}
	
	using graphics = gfx::context;
	
	exception_class(GraphicsException);
	exception_subclass(ContextCreationException, GraphicsException);

	template<class T, useif<is_base_of<gfx::extension, T>::value>>
	boost::optional<T> get(const gfx::context & ctx) {
		return ctx.extension<T>();
	}
}

//---------------------------------------------------------------------------
#endif
