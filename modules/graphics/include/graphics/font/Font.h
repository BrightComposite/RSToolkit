//---------------------------------------------------------------------------

#pragma once

#ifndef FONT_H
#define FONT_H

//---------------------------------------------------------------------------

#include <core/addition/Named.h>
#include <container/Map.h>

#include <io/FileSystem.h>

#include <math/Rect.h>

#include <graphics/image/Image.h>

//---------------------------------------------------------------------------

namespace asd
{
	class Font;
	class Font;
	class Graphics;

	using asd::Font; // specially for Windows Font

	class SymbolData : public ImageData
	{
	public:
		SymbolData() : ImageData() {}
		SymbolData(const SymbolData & data) : ImageData(data), left(data.left), top(data.top), advance(data.advance), character(data.character) {}
		virtual ~SymbolData() {}

		int left = 0, top = 0;
		IntSize advance;
		wchar_t character;
	};

	class Symbol : public Object
	{
		friend class Font;
		friend class Graphics;

	public:
		api(graphics) Symbol(const SymbolData & data);
		api(graphics) Symbol(Graphics * graphics, const SymbolData & data);
		virtual ~Symbol() {}

		int left() const
		{
			return _left;
		}

		int top() const
		{
			return _top;
		}

		const IntSize & advance() const
		{
			return _advance;
		}

		wchar_t character() const
		{
			return _character;
		}

		const Image * image() const
		{
			return _image;
		}

		Graphics * graphics() const
		{
			return _image != nullptr ? _image->graphics() : nullptr;
		}

	protected:
		Handle<Image> _image;
		int _left, _top;
		IntSize _advance;
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

	class Font : public Object
	{
	public:
		Font() {}
		virtual ~Font() {}

		api(graphics) void getSymbol(Handle<Symbol> & output, Graphics * graphics, int size, wchar_t character) const;

		static Handle<Font> api(graphics) load(const path & filepath);

		api(graphics) IntSize getTextSize(Graphics * graphics, const string & text, int fontSize);
		api(graphics) IntSize getTextSize(Graphics * graphics, const wstring & text, int fontSize);

		static api(graphics) void set(const string & name, const Handle<Font> & font, FontStyle style = FontStyle::Regular);
		static api(graphics) void set(const string & name, const Handle<FontFamily> & family);
		static api(graphics) void set(const string & name, const initializer_list<pair<FontStyle, Handle<Font>>> & list);
		static api(graphics) Handle<Font> get(const string & name, FontStyle style = FontStyle::Regular);
		static api(graphics) Handle<Font> obtain(const string & name, const string & path, FontStyle style = FontStyle::Regular);
		static api(graphics) void clear();

	protected:
		virtual Handle<Symbol> & findSymbol(Graphics * graphics, int size, wchar_t character) const = 0;
		virtual void loadSymbol(Handle<Symbol> & symbol, Graphics * graphics, int size, wchar_t character) const = 0;

		static Dictionary<string, FontFamily> & cache()
		{
			static Dictionary<string, FontFamily> _cache;
			return _cache;
		}

		template<class string_t>
		api(graphics) IntSize textSize(Graphics * graphics, const string_t & text, int fontSize);
	};

	struct FontFamily : public Shared
	{
		FontFamily() {}
		FontFamily(const initializer_list<pair<FontStyle, Handle<Font>>> & list) : styles(list.begin(), list.end()) {}

		Dictionary<FontStyle, Font> styles;
	};
}

//---------------------------------------------------------------------------
#endif
