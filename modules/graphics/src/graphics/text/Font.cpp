//---------------------------------------------------------------------------

#include <graphics/text/Font.h>
#include <graphics/text/io/FontIO.h>

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	Symbol::Symbol(Graphics * graphics, const SymbolData & data) :
		_image(graphics->createImage(data)), _left(data.left), _top(data.top), _advance(data.advance), _character(data.character) {}

	Symbol::Symbol(const SymbolData & data) :
		_image(nullptr), _left(data.left), _top(data.top), _advance(data.advance), _character(data.character) {}

	void Font::getSymbol(Handle<Symbol> & output, Graphics * graphics, int size, wchar_t character) const
	{
		auto & s = findSymbol(graphics, size, character);

		if(s == nullptr)
			loadSymbol(s, graphics, size, character);

		output = s;
	}

	Handle<Font> Font::load(const path & filepath)
	{
		Handle<Font> font;
		FontIO::load(font, filepath);

		return font;
	}
}

//---------------------------------------------------------------------------