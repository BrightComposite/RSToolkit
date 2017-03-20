//---------------------------------------------------------------------------

#pragma once

#ifndef SURFACE_H
#define SURFACE_H

//---------------------------------------------------------------------------

#include <core/Exception.h>

#include <graphics/ScreenCoord.h>
#include <graphics/image/Image.h>

//---------------------------------------------------------------------------

namespace asd
{

	class RigidSurfaceException : public Exception
	{
	public:
		RigidSurfaceException() : Exception("Tried to resize rigid surface!") {}
		virtual ~RigidSurfaceException() {}
	};

	class Surface : public Shared
	{
		friend class Graphics;

	public:
		Surface(const IntSize & size) : _viewport(size) {}
		virtual ~Surface() {}

		const Viewport & viewport() const
		{
			return _viewport;
		}

		const IntSize & size() const
		{
			return _viewport.size();
		}

		int width() const
		{
			return _viewport.width();
		}

		int height() const
		{
			return _viewport.height();
		}

		void setSize(const IntSize & size)
		{
			if(size == _viewport.size())
				return;

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
		virtual void requestData(ImageData * data) const = 0;

		Handle<ImageData> requestData() const
		{
			auto data = handle<ImageData>();
			requestData(data);

			return data;
		}

	protected:
		virtual void resize()
		{
			throw RigidSurfaceException();
		}

		Viewport _viewport;
	};

	class ImageSurface : public Surface
	{
	public:
		using Surface::Surface;
		using Surface::clear;

		virtual void addBuffer(Handle<Image> & texture, ImageFormat format = ImageFormat::rgb) = 0;
		virtual void clear(uint bitmask) const = 0; // buffer mask
	};
}

//---------------------------------------------------------------------------
#endif
