//---------------------------------------------------------------------------

#ifndef FONT_IO_H
#define FONT_IO_H

//---------------------------------------------------------------------------

#include <core/Exception.h>
#include <core/container/Map.h>

#include <graphics/text/Font.h>

//---------------------------------------------------------------------------

namespace Rapture
{
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

	class FontIO
	{
	public:
		static void read(Handle<Font> & output, const string & type, const Handle<ByteData> & raw)
		{
			auto & decoder = decoders[type];

			if(decoder == nullptr)
				throw FontDecodingException("Can't read font of type ", type);

			decoder->decode(output, type, raw);
		}

		static void load(Handle<Font> & output, const path & filepath);

		static FontDecoder * getDecoder(const string & type)
		{
			return decoders[type];
		}

		template<class Decoder, useif <
			based_on<Decoder, Singleton<Decoder>>::value,
			based_on<Decoder, FontDecoder>::value
			> endif
		>
		static void setDecoder(const string & type, Type<Decoder> = nullptr)
		{
			decoders[type] = &Decoder::instance();
		}

	protected:
		static map<string, FontDecoder *> decoders;
	};
}

//---------------------------------------------------------------------------
#endif
