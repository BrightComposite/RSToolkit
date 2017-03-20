//---------------------------------------------------------------------------

#include <devil/DevILImageConverter.h>
#include <core/action/Action.h>

#include <il/il.h>
#include <il/ilu.h>

//---------------------------------------------------------------------------

namespace asd
{
	void DevILImageConverter::initialize()
	{
		Type<DevILImageConverter> ThisClass;

		setConverter("bmp", ThisClass);
		setConverter("png", ThisClass);
		setConverter("ico", ThisClass);
		setConverter("tga", ThisClass);
		setConverter("pic", ThisClass);
		setConverter("jpeg", ThisClass);
		setConverter("jpe", ThisClass);
		setConverter("jpg", ThisClass);
	}

	DevILImageConverter::DevILImageConverter()
	{
		ilInit();
		iluInit();

		//ilEnable(IL_ORIGIN_SET);
		//ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	}

	DevILImageConverter::~DevILImageConverter()
	{
		ilShutDown();
	}

	ILenum getILType(const string & extension)
	{
		static map<string, ILenum> types = {
			{"bmp",		IL_BMP},
			{"png",		IL_PNG},
			{"ico",		IL_ICO},
			{"tga",		IL_TGA},
			{"pic",		IL_PIC},
			{"jpeg",	IL_JPG},
			{"jpe",		IL_JPG},
			{"jpg",		IL_JPG}
		};

		return types[extension];
	}

	class DevILException : public Exception
	{
	public:
		DevILException() : Exception("DevIL error:")
		{
			while(true)
			{
				ILenum error = ilGetError();

				if(error = IL_NO_ERROR)
					break;

				data.add(" ", iluErrorString(ilGetError()));
			}
		}
	};

	void DevILImageConverter::decode(ImageData * output, const string & type, const ByteData * raw)
	{
		ILenum internal_type = getILType(type);

		if(ilLoadL(internal_type, raw->data, (uint)raw->size) == IL_FALSE)
			throw DevILException();

		output->width  = ilGetInteger(IL_IMAGE_WIDTH);
		output->height = ilGetInteger(IL_IMAGE_HEIGHT);
		output->bpp    = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

		const byte * data = ilGetData();
		output->set(data, ilGetInteger(IL_IMAGE_SIZE_OF_DATA));
	}

	void DevILImageConverter::encode(OwnedByteData * output, const string & type, const ImageData * image)
	{
		/*ILuint imageId;

		ilGenImages(1, &imageId);
		ilBindImage(imageId);
		FinalAction f(ilDeleteImages, 1, &imageId);*/

		ILenum internal_type = getILType(type);

		if(ilTexImage(image->width, image->height, 0, image->bpp, IL_RGBA, IL_UNSIGNED_BYTE, image->data) == IL_FALSE)
			throw DevILException();

		output->alloc(ilSaveL(internal_type, nullptr, 0));

		if(ilSaveL(internal_type, output->data, (uint)output->size) == IL_FALSE)
			throw DevILException();
	}
}