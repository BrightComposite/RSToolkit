//---------------------------------------------------------------------------

#pragma once

#ifndef SURFACE_H
#define SURFACE_H

//---------------------------------------------------------------------------

#include <core/Exception.h>

#include <graphics/ScreenCoord.h>
#include <graphics/image/image.h>

//---------------------------------------------------------------------------

namespace asd
{
	
	class RigidSurfaceException : public Exception
	{
	public:
		RigidSurfaceException() : Exception("Tried to resize rigid surface!") {}
		
		virtual ~RigidSurfaceException() {}
	};
	
	class surface : public shareable
	{
		friend class graphics;
	
	public:
		surface(const math::int_size & size) : _viewport(size) {}
		
		virtual ~surface() {}
		
		const asd::viewport & viewport() const {
			return _viewport;
		}
		
		const math::int_size & size() const {
			return _viewport.size();
		}
		
		int width() const {
			return _viewport.width();
		}
		
		int height() const {
			return _viewport.height();
		}
		
		void setSize(const math::int_size & size) {
			if(size == _viewport.size()) {
				return;
			}
			
			auto tmp = _viewport;
			_viewport.set(size);
			
			try {
				resize();
			}
			catch(...) {
				_viewport = tmp;
				throw;
			}
		}
		
		virtual void apply() const = 0;
		virtual void present() const = 0;
		virtual void clear() const = 0;
		virtual void requestData(image_data * data) const = 0;
		
		handle <image_data> requestData() const {
			auto data = make::handle<image_data>();
			requestData(data);
			
			return data;
		}
	
	protected:
		virtual void resize() {
			throw RigidSurfaceException();
		}
		
		asd::viewport _viewport;
	};
	
	class ImageSurface : public surface
	{
	public:
		using surface::surface;
		using surface::clear;
		
		virtual void addBuffer(handle <image> & texture, image_format format = image_format::rgb) = 0;
		virtual void clear(uint bitmask) const = 0; // buffer mask
	};
}

//---------------------------------------------------------------------------
#endif
