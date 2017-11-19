//---------------------------------------------------------------------------

#include <freeimage/FreeImageConverter.h>
#include <core/function/method.h>

#include <FreeImage.h>
#include <core/function/anyway.h>

//---------------------------------------------------------------------------

namespace asd
{
	static void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char * message)
	{
		try
		{
			if(fif != FIF_UNKNOWN)
				throw ImageConversionException("FreeImage error with image of format ", FreeImage_GetFormatFromFIF(fif), ": ", message);
			else
				throw ImageConversionException("FreeImage error: ", message);
		}
		catch(const Exception &) {}
	}

	FreeImageConverter::FreeImageConverter()
	{
		FreeImage_Initialise(TRUE);
		FreeImage_SetOutputMessage(FreeImageErrorHandler);
	}

	FreeImageConverter::~FreeImageConverter()
	{
		FreeImage_DeInitialise();
	}

	FREE_IMAGE_FORMAT getFIFFormat(const string & type)
	{
		static dictionary<string, FREE_IMAGE_FORMAT> formats = {
			{"bmp",  FIF_BMP},
			{"ico",  FIF_ICO},
			{"jpe",  FIF_JPEG},
			{"jpeg", FIF_JPEG},
			{"jpg",  FIF_JPEG},
			{"png",  FIF_PNG},
			{"tga",  FIF_TARGA}
		};

		auto f = formats.find(type);

		if(f == formats.end())
			return FREE_IMAGE_FORMAT::FIF_UNKNOWN;

		return f->second;
	}

	void FreeImageConverter::decode(image_data * output, const string & type, const byte_data * raw)
	{
		anyway a;
		FREE_IMAGE_FORMAT internal_format = getFIFFormat(type);

		if(internal_format == FIF_UNKNOWN)
			throw ImageConversionException("Image conversion error: format \"", type, "\" is unknown");

		FIMEMORY * mem = FreeImage_OpenMemory(raw->ptr, static_cast<unsigned long>(raw->size));

		if(!mem)
			throw ImageConversionException("Can't open memory for image");

		a.add(FreeImage_CloseMemory, mem);

		FIBITMAP * bmp = FreeImage_LoadFromMemory(internal_format, mem);

		if(!bmp)
			throw ImageConversionException("Can't read image of format \"", type, "\"");

		a.add(FreeImage_Unload, bmp);

		output->area = {FreeImage_GetWidth(bmp), FreeImage_GetHeight(bmp)};

		const byte bpp = static_cast<byte>(FreeImage_GetBPP(bmp) / 8);

		output->format = preferred_format(bpp);
		output->alloc();

		FreeImage_ConvertToRawBits(output->ptr, bmp, output->width() * bpp, bpp * 8, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	}

	void FreeImageConverter::encode(owned_byte_data * output, const string & type, const image_data * input)
	{
		anyway a;
		FREE_IMAGE_FORMAT internal_format = getFIFFormat(type);

		if(internal_format == FIF_UNKNOWN)
			throw ImageConversionException("Image conversion error: format \"", type, "\" is unknown");
		
		FIMEMORY * mem = FreeImage_OpenMemory();
		
		if(!mem)
			throw ImageConversionException("Can't allocate memory for image");
		
		a.add(FreeImage_CloseMemory, mem);

		uint w = input->width(), h = input->height();
		int bpp, pitch;
		byte * data;

		{
			handle<image_data> buffer(_);
			bpp = preferred_bpp(input->format);

			if(internal_format == FIF_JPEG || bpp < 4)
			{
				input->convert(buffer, image_format::rgb);
				bpp = 3;
			}
			else
				input->convert(buffer, image_format::rgba);

			pitch = w * bpp;
			data = buffer->send();
		}

		FIBITMAP * bmp = FreeImage_ConvertFromRawBitsEx(
			FALSE, data, FIT_BITMAP,
			w, h, pitch, bpp * 8,
			FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE
			);

		if(!bmp)
			throw ImageConversionException("Can't convert raw data to format \"", type, "\"");

		a.add(FreeImage_Unload, bmp);

		if(!FreeImage_SaveToMemory(internal_format, bmp, mem))
			throw ImageConversionException("Can't save bitmap of format \"", type, "\" to memory");

		DWORD size;

		FreeImage_AcquireMemory(mem, &data, &size);
		output->set(data, size);
	}
}
