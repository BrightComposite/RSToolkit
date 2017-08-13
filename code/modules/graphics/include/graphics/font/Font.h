//---------------------------------------------------------------------------

#pragma once

#ifndef FONT_H
#define FONT_H

//---------------------------------------------------------------------------

#include <core/addition/named.h>
#include <container/Map.h>

#include <io/FileSystem.h>

#include <math/rect.h>

#include <graphics/image/image.h>

//---------------------------------------------------------------------------

namespace asd
{
	class Font;
	class Font;
	class graphics;

	using asd::Font; // specially for Windows Font

	class SymbolData : public image_data
	{
	public:
		SymbolData() : image_data() {}
		SymbolData(const SymbolData & data) : image_data(data), left(data.left), top(data.top), advance(data.advance), character(data.character) {}
		virtual ~SymbolData() {}

		int left = 0, top = 0;
		math::int_size advance;
		wchar_t character;
	};

	class Symbol : public object
	{
		friend class Font;
		friend class graphics;

	public:
		api(graphics) Symbol(const SymbolData & data);
		api(graphics) Symbol(asd::graphics * graphics, const SymbolData & data);
		virtual ~Symbol() {}

		int left() const
		{
			return _left;
		}

		int top() const
		{
			return _top;
		}

		const math::int_size & advance() const
		{
			return _advance;
		}

		wchar_t character() const
		{
			return _character;
		}

		const asd::image * image() const
		{
			return _image;
		}

		asd::graphics * graphics() const
		{
			return _image != nullptr ? _image->graphics() : nullptr;
		}

	protected:
		handle<asd::image> _image;
		int _left, _top;
		math::int_size _advance;
		wchar_t _character;
	};

	struct FontFamily;

	enum class FontStyle : int
	{
		Regular,
		Italic,
		Bold,
		BoldItalic
	};

	template<>
	use_enum_hash(FontStyle);

	class Font : public object
	{
	public:
		Font() {}
		virtual ~Font() {}

		api(graphics) void getSymbol(handle<Symbol> & output, graphics * graphics, int size, wchar_t character) const;

		static handle<Font> api(graphics) load(const path & filepath);

		api(graphics) math::int_size getTextSize(graphics * graphics, const string & text, int fontSize);
		api(graphics) math::int_size getTextSize(graphics * graphics, const wstring & text, int fontSize);

		static api(graphics) void set(const string & name, const handle<Font> & font, FontStyle style = FontStyle::Regular);
		static api(graphics) void set(const string & name, const handle<FontFamily> & family);
		static api(graphics) void set(const string & name, const initializer_list<pair<FontStyle, handle<Font>>> & list);
		static api(graphics) handle<Font> get(const string & name, FontStyle style = FontStyle::Regular);
		static api(graphics) handle<Font> obtain(const string & name, const string & path, FontStyle style = FontStyle::Regular);
		static api(graphics) void clear();

	protected:
		virtual handle<Symbol> & findSymbol(graphics * graphics, int size, wchar_t character) const = 0;
		virtual void loadSymbol(handle<Symbol> & symbol, graphics * graphics, int size, wchar_t character) const = 0;

		static Dictionary<string, FontFamily> & cache()
		{
			static Dictionary<string, FontFamily> _cache;
			return _cache;
		}

		template<class string_t>
		api(graphics) math::int_size textSize(graphics * graphics, const string_t & text, int fontSize);
	};

	struct FontFamily : public shareable
	{
		FontFamily() {}
		FontFamily(const initializer_list<pair<FontStyle, handle<Font>>> & list) : styles(list.begin(), list.end()) {}

		Dictionary<FontStyle, Font> styles;
	};
}

//---------------------------------------------------------------------------
#endif
