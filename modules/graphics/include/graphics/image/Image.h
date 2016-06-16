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
		Image(Graphics * graphics, const ImageData & data) : _format(data.format), _width(data.width), _height(data.height), _graphics(graphics) {}
		virtual ~Image() {}

		uint width() const
		{
			return _width;
		}

		uint height() const
		{
			return _height;
		}

		Graphics * graphics() const
		{
			return _graphics;
		}

		virtual void apply() const = 0;
		virtual void requestData(ImageData * output) const = 0;

		static Handle<Image> api(graphics) load(Graphics * graphics, const path & filepath);
		static void api(graphics) save(const path & filepath, const Image * image);

	protected:
		uint _width, _height;
		ImageFormat _format;
		Graphics * _graphics;
	};
}

//---------------------------------------------------------------------------
#endif