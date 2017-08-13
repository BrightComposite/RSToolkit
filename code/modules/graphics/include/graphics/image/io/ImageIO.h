//---------------------------------------------------------------------------

#pragma once

#ifndef IMAGE_CONVERTER_H
#define IMAGE_CONVERTER_H

//---------------------------------------------------------------------------

#include <graphics/image/image_data.h>
#include <container/Map.h>
#include <core/addition/singleton.h>
#include <core/String.h>
#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace asd
{
	class ImageIO;
	apistruct(graphics) singleton<ImageIO>;

	class ImageDecoder
	{
		friend class ImageIO;

	public:
		virtual ~ImageDecoder() {}

	protected:
		virtual void decode(image_data * output, const string & type, const ByteData * raw) = 0;
	};

	class ImageEncoder
	{
		friend class ImageIO;

	public:
		virtual ~ImageEncoder() {}

	protected:
		virtual void encode(OwnedByteData * output, const string & type, const image_data * image) = 0;
	};

	class ImageConverter : public ImageDecoder, public ImageEncoder
	{
	public:
		virtual ~ImageConverter() {}
	};

	class ImageConversionException : public Exception
	{
	public:
		using Exception::Exception;
		virtual ~ImageConversionException() {}
	};

	class ImageReadException : public ImageConversionException
	{
	public:
		ImageReadException(const string & type) : ImageConversionException("Can't read image data of type ", type) {}
		virtual ~ImageReadException() {}
	};

	class ImageWriteException : public ImageConversionException
	{
	public:
		ImageWriteException(const string & type) : ImageConversionException("Can't write image data of type ", type) {}
		virtual ~ImageWriteException() {}
	};

	class ImageIO : public singleton<ImageIO>
	{
	public:
		static void read(image_data * output, const string & type, const ByteData * raw)
		{
			auto & decoder = instance().decoders[type];

			if(decoder == nullptr)
				throw ImageReadException(type);

			decoder->decode(output, type, raw);
		}

		static void write(OwnedByteData * output, const string & type, const image_data * image)
		{
			auto & encoder = instance().encoders[type];

			if(encoder == nullptr)
				throw ImageWriteException(type);

			encoder->encode(output, type, image);
		}

		static api(graphics) void load(image_data * output, const path & filepath);
		static api(graphics) void save(const path & filepath, const image_data * image);

		static ImageDecoder * getDecoder(const string & type)
		{
			return instance().decoders[type];
		}

		static ImageEncoder * getEncoder(const string & type)
		{
			return instance().encoders[type];
		}

		template<class Decoder, useif<
			based_on<Decoder, singleton<Decoder>>::value,
			based_on<Decoder, ImageDecoder>::value
			>
		>
		static void setDecoder(const string & type, Type<Decoder> = {})
		{
			instance().decoders[type] = &Decoder::instance();
		}

		template<class Encoder, useif<
			based_on<Encoder, singleton<Encoder>>::value,
			based_on<Encoder, ImageEncoder>::value
			>
		>
		static void setEncoder(const string & type, Type<Encoder> = {})
		{
			instance().encoders[type] = &Encoder::instance();
		}

		template<class Converter, useif<
			based_on<Converter, singleton<Converter>>::value,
			based_on<Converter, ImageDecoder>::value,
			based_on<Converter, ImageEncoder>::value
			>
		>
		static void setConverter(const string & type, Type<Converter> = {})
		{
			auto & inst = instance();
			inst.decoders[type] = &Converter::instance();
			inst.encoders[type] = &Converter::instance();
		}

	protected:
		dictionary<string, ImageDecoder *> decoders;
		dictionary<string, ImageEncoder *> encoders;
	};
}

//---------------------------------------------------------------------------
#endif
