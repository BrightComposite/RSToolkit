//---------------------------------------------------------------------------

#pragma once

#ifndef FONT_IO_H
#define FONT_IO_H

//---------------------------------------------------------------------------

#include <core/Exception.h>
#include <container/Map.h>

#include <graphics/font/Font.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class FontIO;
	api_struct(graphics, Singleton<FontIO>);

	class FontDecoder
	{
		friend class FontIO;

	public:
		virtual ~FontDecoder() {}

	protected:
		virtual void decode(Handle<Font> & output, const string & type, const Handle<ByteData> & raw) = 0;
	};

	class FontDecodingException : public Exception
	{
	public:
		using Exception::Exception;
		virtual ~FontDecodingException() {}
	};

	class FontIO : public Singleton<FontIO>
	{
	public:
		static void read(Handle<Font> & output, const string & type, const Handle<ByteData> & raw)
		{
			auto & decoder = instance().decoders[type];

			if(decoder == nullptr)
				throw FontDecodingException("Can't read font of type ", type);

			decoder->decode(output, type, raw);
		}

		static void load(Handle<Font> & output, const path & filepath);

		static FontDecoder * getDecoder(const string & type)
		{
			return instance().decoders[type];
		}

		template<class Decoder, useif<
			based_on<Decoder, Singleton<Decoder>>::value,
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
