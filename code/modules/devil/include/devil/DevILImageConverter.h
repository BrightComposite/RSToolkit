//---------------------------------------------------------------------------

#ifndef DEVIL_IMAGE_HANDLER_H
#define DEVIL_IMAGE_HANDLER_H

//---------------------------------------------------------------------------

#include <graphics/image/ImageConverter.h>
#include <core/container/Map.h>
#include <core/String.h>

//---------------------------------------------------------------------------

namespace asd
{
	class DevILImageConverter : public ImageConverter, public Singleton<DevILImageConverter>
	{
	public:
		DevILImageConverter();
		virtual ~DevILImageConverter();

		virtual void decode(ImageData * output, const string & type, const ByteData * raw) override;
		virtual void encode(OwnedByteData * output, const string & type, const ImageData * image) override;

		static void initialize();
	};
}

//---------------------------------------------------------------------------
#endif