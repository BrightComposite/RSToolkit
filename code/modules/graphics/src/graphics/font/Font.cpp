//---------------------------------------------------------------------------

#include <graphics/font/Font.h>
#include <graphics/font/io/FontIO.h>

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace asd
{
	Symbol::Symbol(asd::graphics * graphics, const SymbolData & data) :
		_image(graphics->createImage(data)), _left(data.left), _top(data.top), _advance(data.advance), _character(data.character) {}

	Symbol::Symbol(const SymbolData & data) :
		_image(nullptr), _left(data.left), _top(data.top), _advance(data.advance), _character(data.character) {}

	void Font::getSymbol(handle<Symbol> & output, graphics * graphics, int size, wchar_t character) const
	{
		auto & s = findSymbol(graphics, size, character);

		if(s == nullptr)
			loadSymbol(s, graphics, size, character);

		output = s;
	}

	handle<Font> Font::load(const path & filepath)
	{
		handle<Font> font;
		FontIO::load(font, filepath);

		return font;
	}

	math::int_size Font::getTextSize(asd::graphics * graphics, const string & text, int fontSize)
	{
		return textSize<string>(graphics, text, fontSize);
	}
	
	math::int_size Font::getTextSize(asd::graphics * graphics, const wstring & text, int fontSize)
	{
		return textSize<wstring>(graphics, text, fontSize);
	}

	template<class string_t>
	math::int_size Font::textSize(asd::graphics * graphics, const string_t & text, int fontSize)
	{
		handle<Symbol> symbol;
		math::int_size size;

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

	template math::int_size Font::textSize<string>(graphics * graphics, const string & text, int fontSize);
	template math::int_size Font::textSize<wstring>(graphics * graphics, const wstring & text, int fontSize);

	void Font::set(const string & name, const handle<Font> & font, FontStyle style)
	{
		auto & family = cache()[name];

		if(family == nullptr)
			family.init();

		family->styles[style] = font;
	}

	void Font::set(const string & name, const handle<FontFamily> & family)
	{
		cache()[name] = family;
	}

	void Font::set(const string & name, const initializer_list<pair<FontStyle, handle<Font>>> & list)
	{
		cache()[name].init(list);
	}

	handle<Font> Font::get(const string & name, FontStyle style)
	{
		auto & family = cache()[name];

		if(family == nullptr)
			return nullptr;

		return family->styles[style];
	}

	handle<Font> Font::obtain(const string & name, const string & path, FontStyle style)
	{
		auto & family = cache()[name];

		if(family == nullptr)
			family.init();

		auto & font = family->styles[style];

		if(font == nullptr)
			font = Font::load(path);

		return font;
	}

	void Font::clear()
	{
		return cache().clear();
	}
}

//---------------------------------------------------------------------------