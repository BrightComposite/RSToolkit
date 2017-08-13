//---------------------------------------------------------------------------

#pragma once

#ifndef FREE_IMAGE_CONVERTER_H
#define FREE_IMAGE_CONVERTER_H

//---------------------------------------------------------------------------

#include <graphics/image/io/ImageIO.h>
#include <container/Map.h>
#include <core/String.h>

//---------------------------------------------------------------------------

namespace asd
{
	class FreeImageConverter : public ImageConverter, public singleton<FreeImageConverter>
	{
	public:
		api(freeimage) FreeImageConverter();
		virtual api(freeimage) ~FreeImageConverter();

		virtual api(freeimage) void decode(image_data * output, const string & type, const ByteData * raw) override;
		virtual api(freeimage) void encode(OwnedByteData * output, const string & type, const image_data * image) override;

		static void initialize()
		{
			auto ThisClass = gettype(FreeImageConverter);

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