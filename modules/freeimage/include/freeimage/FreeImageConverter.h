//---------------------------------------------------------------------------

#ifndef FREE_IMAGE_CONVERTER_H
#define FREE_IMAGE_CONVERTER_H

//---------------------------------------------------------------------------

#include <graphics/image/io/ImageIO.h>
#include <core/container/Map.h>
#include <core/String.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class FreeImageConverter : public ImageConverter, public Singleton<FreeImageConverter>
	{
	public:
		FreeImageConverter();
		virtual ~FreeImageConverter();

		virtual void decode(ImageData * output, const string & type, const ByteData * raw) override;
		virtual void encode(OwnedByteData * output, const string & type, const ImageData * image) override;

		static void initialize()
		{
			Type<FreeImageConverter> ThisClass;

			ImageIO::setConverter("bmp", ThisClass);
			ImageIO::setConverter("ico", ThisClass);
			ImageIO::setConverter("jpe", ThisClass);
			ImageIO::setConverter("jpeg", ThisClass);
			ImageIO::setConverter("jpg", ThisClass);
			ImageIO::setConverter("png", ThisClass);
			ImageIO::setConverter("tga", ThisClass);
		}
	};
}

//---------------------------------------------------------------------------
#endif