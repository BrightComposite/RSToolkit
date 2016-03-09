//---------------------------------------------------------------------------

#include <graphics/image/Image.h>
#include <graphics/image/io/ImageIO.h>
#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	Handle<Image> Image::load(Graphics * graphics, const path & filepath)
	{
		auto data = handle<ImageData>();
		ImageIO::load(data, filepath);

		return graphics->createImage(*data);
	}

	void Image::save(const path & filepath, const Image * image)
	{
		auto data = handle<ImageData>();
		image->requestData(data);

		ImageIO::save(filepath, data);
	}
}