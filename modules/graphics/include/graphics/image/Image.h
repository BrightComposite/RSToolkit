//---------------------------------------------------------------------------

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
		Image(const Graphics * graphics, const ImageData & data) : _format(data.format), _width(data.width), _height(data.height), _graphics(graphics) {}

		uint width() const
		{
			return _width;
		}

		uint height() const
		{
			return _height;
		}

		const Graphics * graphics() const
		{
			return _graphics;
		}

		virtual void apply() const = 0;
		virtual void requestData(ImageData * output) const = 0;

		static Handle<Image> load(Graphics * graphics, const path & filepath);
		static void save(const path & filepath, const Image * image);

	protected:
		uint _width, _height;
		ImageFormat _format;
		const Graphics * _graphics;
	};
}

//---------------------------------------------------------------------------
#endif