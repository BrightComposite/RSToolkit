//---------------------------------------------------------------------------

#pragma once

#ifndef FONT_IO_H
#define FONT_IO_H

//---------------------------------------------------------------------------

#include <core/Exception.h>
#include <container/Map.h>

#include <graphics/font/Font.h>

#include <core/addition/singleton.h>

//---------------------------------------------------------------------------

namespace asd
{
	class FontIO;
	apistruct(graphics) singleton<FontIO>;

	class FontDecoder
	{
		friend class FontIO;

	public:
		virtual ~FontDecoder() {}

	protected:
		virtual void decode(handle<Font> & output, const string & type, const handle<ByteData> & raw) = 0;
	};

	class FontDecodingException : public Exception
	{
	public:
		using Exception::Exception;
		virtual ~FontDecodingException() {}
	};

	class FontIO : public singleton<FontIO>
	{
	public:
		static void read(handle<Font> & output, const string & type, const handle<ByteData> & raw)
		{
			auto & decoder = instance().decoders[type];

			if(decoder == nullptr)
				throw FontDecodingException("Can't read a font of the type ", type, "! Please, provide a decoder for this type.");

			decoder->decode(output, type, raw);
		}

		static void load(handle<Font> & output, const path & filepath);

		static FontDecoder * getDecoder(const string & type)
		{
			return instance().decoders[type];
		}

		template<class Decoder, useif<
			based_on<Decoder, singleton<Decoder>>::value,
			based_on<Decoder, FontDecoder>::value
			>
		>
		static void setDecoder(const string & type, Type<Decoder> = {})
		{
			instance().decoders[type] = &Decoder::instance();
		}

	protected:
		dictionary<string, FontDecoder *> decoders;
	};
}

//---------------------------------------------------------------------------
#endif
