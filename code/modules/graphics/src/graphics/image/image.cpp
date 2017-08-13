//---------------------------------------------------------------------------

#include <graphics/image/image.h>
#include <graphics/image/io/ImageIO.h>
#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace asd
{
	handle<image> image::load(asd::graphics * graphics, const path & filepath)
	{
		auto data = make::handle<image_data>();
		ImageIO::load(data, filepath);

		return graphics->createImage(*data);
	}

	void image::save(const path & filepath, const image * image)
	{
		auto data = make::handle<image_data>();
		image->requestData(data);

		ImageIO::save(filepath, data);
	}
}