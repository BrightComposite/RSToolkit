//---------------------------------------------------------------------------

#pragma once

#ifndef FREETYPE_DECODER_H
#define FREETYPE_DECODER_H

//---------------------------------------------------------------------------

#include <graphics/font/io/FontIO.h>
#include <container/map.h>
#include <core/String.h>

//---------------------------------------------------------------------------

namespace asd
{
	class FreeTypeDecoder : public FontDecoder, public singleton<FreeTypeDecoder>
	{
	public:
		api(freetype) FreeTypeDecoder();
		virtual api(freetype) ~FreeTypeDecoder();

		virtual api(freetype) void decode(handle<Font> & output, const string & type, const handle<ByteData> & raw) override;

		static void initialize()
		{
			FontIO::setDecoder("ttf", gettype(FreeTypeDecoder));
		}
	};

	template struct api(freetype) singleton<FreeTypeDecoder>;
}

//---------------------------------------------------------------------------
#endif
