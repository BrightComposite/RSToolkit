//---------------------------------------------------------------------------

#pragma once

#ifndef TEXT_H
#define TEXT_H

//---------------------------------------------------------------------------

#include <ui/Widget.h>

#include <graphics/Color.h>
#include <graphics/Surface.h>
#include <graphics/font/Font.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Text;
	class TextLayer;
	class StaticTextLayer;
	class SmartTextLayer;

	using TextComponent = WidgetLayerComponent<TextLayer>;

	class Text : public Shared
	{
	public:
		api(ui) Text(Graphics * graphics, const WideString & s, const Handle<Font> & font, const Color & color = Color::black, int fontSize = 14);

		Handle<Text> clone() const
		{
			return Handle<Text>(_graphics, _contents, _font, _color, _fontSize);
		}

		Handle<Text> clone(Graphics * graphics) const
		{
			return Handle<Text>(graphics, _contents, _font, _color, _fontSize);
		}

		const WideString & contents() const
		{
			return _contents;
		}

		const IntSize & size() const
		{
			return _size;
		}

		virtual void setContents(const WideString & contents)
		{
			_contents = contents;
			_size = _font->getTextSize(_graphics, _contents, _fontSize);
		}

		virtual api(ui) void draw(const IntPoint & pos);

		template<class TextLayerClass = StaticTextLayer, useif<based_on<TextLayerClass, TextLayer>::value>>
		static void set(Widget * w, const WideString & s, const Handle<Font> & font, const Color & color = Color::black, int fontSize = 14)
		{
			w->require<TextComponent>()->set(Unique<TextLayerClass>(w, Handle<Text>(w->graphics(), s, font, color, fontSize)));
		}

		static api(ui) void setDynamic(Widget * w, const WideString & s, const Handle<Font> & font, const Color & color = Color::black, int fontSize = 14);
		static api(ui) void setSmart(Widget * w, const WideString & s, const Handle<Font> & font, const Color & color = Color::black, int fontSize = 14);
		static api(ui) void setContents(Widget * w, const WideString & s);
		static api(ui) TextLayer * get(Widget * w);
		static api(ui) void clear(Widget * w);

	protected:
		WideString _contents;

		Graphics * _graphics;
		IntSize _size;
		Color _color;
		Handle<Font> _font;
		int _fontSize = 14;
	};

	class TextLayer : public WidgetLayer
	{
		friend_handle;

	public:
		api(ui) TextLayer(Widget * w, Unique<Text> && text);

		TextLayer(Widget * w, const WideString & s, const Handle<Font> & font, const Color & color = Color::black, int fontSize = 14) :
			TextLayer(w, Unique<Text>(w->graphics(), s, font, color, fontSize)) {}

		virtual ~TextLayer() {}

		virtual Handle<WidgetLayer> clone(Widget * widget) const override
		{
			return Handle<TextLayer>(widget, _text->clone());
		}

		const WideString & contents() const
		{
			return _text->contents();
		}

		Text * text() const
		{
			return _text;
		}

		void setContents(const WideString & contents)
		{
			_text->setContents(contents);
			update();
		}

		void setText(Unique<Text> && contents)
		{
			_text = move(contents);
			update();
		}

	protected:
		virtual api(ui) void draw(Widget * w) override;
		virtual void update() {}

		Graphics * _graphics;
		Unique<Text> _text;
	};

	create_layer_component(ui, TextLayer);

	class StaticTextLayer : public TextLayer
	{
	public:
		api(ui) StaticTextLayer(Widget * w, Unique<Text> && text);

		StaticTextLayer(Widget * w, const WideString & s, const Handle<Font> & font, const Color & color = Color::black, int fontSize = 14) :
			StaticTextLayer(w, Unique<Text>(w->graphics(), s, font, color, fontSize)) {}

		virtual ~StaticTextLayer() {}

		virtual Handle<WidgetLayer> clone(Widget * widget) const override
		{
			return Handle<StaticTextLayer>(widget, _text->clone());
		}

		void requestData(ImageData * data)
		{
			_image->requestData(data);
		}

	protected:
		virtual api(ui) void draw(Widget * w) override;
		virtual api(ui) void update() override;

		Handle<Image> _image = nullptr;
		Handle<ImageSurface> _surface = nullptr;
	};

	class SmartTextLayer : public StaticTextLayer
	{
	public:
		SmartTextLayer(Widget * w, Unique<Text> && text) : StaticTextLayer(w, move(text)), drawer(dynamicDraw) {}

		SmartTextLayer(Widget * w, const WideString & s, const Handle<Font> & font, const Color & color = Color::black, int fontSize = 14) :
			SmartTextLayer(w, Unique<Text>(w->graphics(), s, font, color, fontSize)) {}

		virtual ~SmartTextLayer() {}

		virtual Handle<WidgetLayer> clone(Widget * widget) const override
		{
			return Handle<SmartTextLayer>(widget, _text->clone());
		}

	protected:
		using drawer_t = void(*)(SmartTextLayer * layer, Widget * w);

		static api(ui) void dynamicDraw(SmartTextLayer * layer, Widget * w);
		static api(ui) void staticDraw(SmartTextLayer * layer, Widget * w);

		virtual api(ui) void draw(Widget * w) override;
		virtual api(ui) void update() override;

		drawer_t drawer;
		int dynamicCounter;
	};
}

//---------------------------------------------------------------------------
#endif