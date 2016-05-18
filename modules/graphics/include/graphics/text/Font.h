//---------------------------------------------------------------------------

#ifndef FONT_H
#define FONT_H

//---------------------------------------------------------------------------

#include <core/addition/Named.h>
#include <core/container/Map.h>

#include <io/FileSystem.h>

#include <math/Rect.h>

#include <graphics/image/Image.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics;

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
		friend class Graphics;

	public:
		Symbol(const SymbolData & data);
		Symbol(Graphics * graphics, const SymbolData & data);
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

	class Font : public Object
	{
	public:
		Font() {}
		virtual ~Font() {}

		void getSymbol(Handle<Symbol> & output, Graphics * graphics, int size, wchar_t character) const;

		static Handle<Font> load(const path & filepath);

	protected:
		virtual Handle<Symbol> & findSymbol(Graphics * graphics, int size, wchar_t character) const = 0;
		virtual void loadSymbol(Handle<Symbol> & symbol, Graphics * graphics, int size, wchar_t character) const = 0;
	};

	using Rapture::Font; // specially for Windows Font

	enum class FontStyle : int
	{
		Regular,
		Italic,
		Bold,
		BoldItalic
	};
}

namespace std
{
	template<>
	use_enum_hash(Rapture::FontStyle);
}

namespace Rapture
{
	struct FontFamily : public Shared
	{
		FontFamily() {}
		FontFamily(const initializer_list<pair<FontStyle, Handle<Font>>> & list) : styles(list.begin(), list.end()) {}

		Map<FontStyle, Font> styles;
	};

	class FontCache : public Singleton<FontCache>
	{
	public:
		static void set(const string & name, const Handle<Font> & font, FontStyle style = FontStyle::Regular)
		{
			auto & family = instance()._cache[name];

			if(family == nullptr)
				family.init();

			family->styles[style] = font;
		}

		static void set(const string & name, const Handle<FontFamily> & family)
		{
			instance()._cache[name] = family;
		}

		static void set(const string & name, const initializer_list<pair<FontStyle, Handle<Font>>> & list)
		{
			instance()._cache[name].init(list);
		}

		static Handle<Font> get(const string & name, FontStyle style = FontStyle::Regular)
		{
			auto & family = instance()._cache[name];

			if(family == nullptr)
				return nullptr;

			return family->styles[style];
		}

		static void clear()
		{
			return instance()._cache.clear();
		}

	protected:
		Map<string, FontFamily> _cache;
	};
}

//---------------------------------------------------------------------------
#endif
