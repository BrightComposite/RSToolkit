//---------------------------------------------------------------------------

#ifndef FREETYPE_DECODER_H
#define FREETYPE_DECODER_H

//---------------------------------------------------------------------------

#include <graphics/text/io/FontIO.h>
#include <core/container/Map.h>
#include <core/String.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class FreeTypeDecoder : public FontDecoder, public Singleton<FreeTypeDecoder>
	{
	public:
		FreeTypeDecoder();
		virtual ~FreeTypeDecoder();

		virtual void decode(Handle<Font> & output, const string & type, const Handle<ByteData> & raw) override;

		static void initialize()
		{
			FontIO::setDecoder("ttf", gettype(FreeTypeDecoder));
		}
	};
}

//---------------------------------------------------------------------------
#endif
