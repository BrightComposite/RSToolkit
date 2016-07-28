//---------------------------------------------------------------------------

#include <graphics/font/Font.h>
#include <graphics/font/io/FontIO.h>

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

	IntSize Font::getTextSize(Graphics * graphics, const string & text, int fontSize)
	{
		return textSize<string>(graphics, text, fontSize);
	}

	IntSize Font::getTextSize(Graphics * graphics, const wstring & text, int fontSize)
	{
		return textSize<wstring>(graphics, text, fontSize);
	}

	template<class string_t>
	IntSize Font::textSize(Graphics * graphics, const string_t & text, int fontSize)
	{
		Handle<Symbol> symbol;
		IntSize size;

		getSymbol(symbol, graphics, fontSize, ' ');
		int space = symbol->_advance.x;
		int tab = space * 4;

		for(size_t i = 0; i < text.length(); ++i)
		{
			auto & ch = text[i];

			switch(ch)
			{
				case '\t':
					size.x = ((size.x + space) / tab + 1) * tab;
					break;

				case ' ':
					size.x += space;
					break;

				default:
					if(ch < ' ')
						break;

					getSymbol(symbol, graphics, fontSize, ch);
					size.x += symbol->_advance.x;

					if(symbol->_image != nullptr)
					{
						int h = symbol->_image->height() + symbol->_top;

						if(h > size.y)
							size.y = h;

						if(i == text.length() - 1)
							size.x += std::max(0, static_cast<int>(symbol->_image->width()) - symbol->_advance.x);
					}
			}
		}

		return size;
	}

	template IntSize Font::textSize<string>(Graphics * graphics, const string & text, int fontSize);
	template IntSize Font::textSize<wstring>(Graphics * graphics, const wstring & text, int fontSize);
}

//---------------------------------------------------------------------------