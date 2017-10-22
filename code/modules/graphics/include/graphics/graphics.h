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
		class painter {
			morph_origin(painter)
		};
		
		template <class T, useif<is_base_of<painter, T>::value>>
		morph_id_t painter_id = morph_id<T>;
		
		class context
		{
		public:
			virtual ~context() {}

			template <class T>
			boost::optional<T> painter() {
				auto i = _painters.find(painter_id<T>);
				return i != _painters.end() ? i.value() : {};
			}
			
			virtual void draw() = 0;
			virtual void flush() {
				draw();
			}

		protected:
			map<morph_id_t, gfx::painter> _painters;
		};
		
		template <class Gfx>
		class driver_context : public context {};

		template <class Gfx>
		class driver
		{
		public:
			using context_type = driver_context<Gfx>;
		};
	}
	
	using graphics = gfx::context;
	
	exception_class(GraphicsException);
	exception_subclass(ContextCreationException, GraphicsException);
}

//---------------------------------------------------------------------------
#endif
