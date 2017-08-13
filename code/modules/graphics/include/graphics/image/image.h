//---------------------------------------------------------------------------

#pragma once

#ifndef IMAGE_H
#define IMAGE_H

//---------------------------------------------------------------------------

#include <core/object.h>
#include <graphics/image/image_data.h>

//---------------------------------------------------------------------------

namespace asd
{
	class graphics;

	class image : public object
	{
	public:
		image(asd::graphics * graphics, uint width, uint height, image_format format) : _format(format), _size(width, height), _graphics(graphics) {}
		image(asd::graphics * graphics, const image_data & data) : _format(data.format), _size(data.area), _graphics(graphics) {}
		virtual ~image() {}

		const math::uint_size & size() const
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
		
		asd::graphics * graphics() const
		{
			return _graphics;
		}

		virtual void apply() const = 0;
		virtual void requestData(image_data * output) const = 0;

		static handle<image> api(graphics) load(asd::graphics * graphics, const path & filepath);
		static api(graphics) void save(const path & filepath, const image * image);

	protected:
		image_format _format;
		math::uint_size _size;
		asd::graphics * _graphics;
	};
}

//---------------------------------------------------------------------------
#endif