//---------------------------------------------------------------------------

#pragma once

#ifndef IMAGE_H
#define IMAGE_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <graphics/image/ImageData.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics;

	class Image : public Object
	{
	public:
		Image(Graphics * graphics, uint width, uint height, ImageFormat format) : _format(format), _size(width, height), _graphics(graphics) {}
		Image(Graphics * graphics, const ImageData & data) : _format(data.format), _size(data.area), _graphics(graphics) {}
		virtual ~Image() {}

		const UintSize & size() const
		{
			return _size;
		}

		uint width() const
		{
			return _size.x;
		}

		uint height() const
		{
			return _size.y;
		}

		Graphics * graphics() const
		{
			return _graphics;
		}

		virtual void apply() const = 0;
		virtual void requestData(ImageData * output) const = 0;

		static Handle<Image> api(graphics) load(Graphics * graphics, const path & filepath);
		static api(graphics) void save(const path & filepath, const Image * image);

	protected:
		UintSize _size;
		ImageFormat _format;
		Graphics * _graphics;
	};
}

//---------------------------------------------------------------------------
#endif