//---------------------------------------------------------------------------

#pragma once

#ifndef TEXT_H
#define TEXT_H

//---------------------------------------------------------------------------

#include <ui/widget.h>

#include <graphics/Color.h>
#include <graphics/surface.h>
#include <graphics/font/Font.h>

//---------------------------------------------------------------------------

namespace asd
{
	class Text;
	class TextLayer;
	class StaticTextLayer;
	class SmartTextLayer;

	using TextComponent = WidgetLayerComponent<TextLayer>;

	class Text : public shareable<Text>
	{
	public:
		api(ui) Text(asd::graphics * graphics, const WideString & s, const handle<Font> & font, const Color & color = Color::black, int fontSize = 14);
		virtual ~Text() {}
		
		handle<Text> clone() const
		{
			return make::handle<Text>(_graphics, _contents, _font, _color, _fontSize);
		}

		handle<Text> clone(graphics * graphics) const
		{
			return make::handle<Text>(graphics, _contents, _font, _color, _fontSize);
		}

		const WideString & contents() const
		{
			return _contents;
		}

		const int_size & size() const
		{
			return _size;
		}

		virtual void setContents(const WideString & contents)
		{
			_contents = contents;
			_size = _font->getTextSize(_graphics, _contents, _fontSize);
		}

		virtual api(ui) void draw(const int_point & pos);

		template<class TextLayerClass = StaticTextLayer, useif<based_on<TextLayerClass, TextLayer>::value>>
		static void set(widget * w, const WideString & s, const handle<Font> & font, const Color & color = Color::black, int fontSize = 14);

		static api(ui) void setDynamic(widget * w, const WideString & s, const handle<Font> & font, const Color & color = Color::black, int fontSize = 14);
		static api(ui) void setSmart(widget * w, const WideString & s, const handle<Font> & font, const Color & color = Color::black, int fontSize = 14);
		static api(ui) void setContents(widget * w, const WideString & s);
		static api(ui) TextLayer * get(widget * w);
		static api(ui) void clear(widget * w);

	protected:
		graphics * _graphics;
		WideString _contents;

		handle<Font> _font;
		Color _color;
		int _fontSize = 14;
		int_size _size;
	};

	class TextLayer : public WidgetLayer
	{
		friend_handle;

	public:
		api(ui) TextLayer(widget * w, unique<Text> && text);

		TextLayer(widget * w, const WideString & s, const handle<Font> & font, const Color & color = Color::black, int fontSize = 14) :
			TextLayer(w, make::unique<Text>(w->graphics(), s, font, color, fontSize)) {}

		virtual ~TextLayer() {}

		virtual handle<WidgetLayer> clone(widget * widget) const override
		{
			return handle<TextLayer>(widget, _text->clone());
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

		void setText(unique<Text> && contents)
		{
			_text = move(contents);
			update();
		}

	protected:
		virtual api(ui) void draw(widget * w) override;
		virtual void update() {}

		graphics * _graphics;
		unique<Text> _text;
	};

	create_layer_component(ui, TextLayer);

	class StaticTextLayer : public TextLayer
	{
	public:
		api(ui) StaticTextLayer(widget * w, unique<Text> && text);

		StaticTextLayer(widget * w, const WideString & s, const handle<Font> & font, const Color & color = Color::black, int fontSize = 14) :
			StaticTextLayer(w, unique<Text>(w->graphics(), s, font, color, fontSize)) {}

		virtual ~StaticTextLayer() {}

		virtual handle<WidgetLayer> clone(widget * widget) const override
		{
			return handle<StaticTextLayer>(widget, _text->clone());
		}

		void requestData(image_data * data)
		{
			_image->requestData(data);
		}

	protected:
		virtual api(ui) void draw(widget * w) override;
		virtual api(ui) void update() override;

		handle<image> _image = nullptr;
		handle<ImageSurface> _surface = nullptr;
	};

	class SmartTextLayer : public StaticTextLayer
	{
	public:
		SmartTextLayer(widget * w, unique<Text> && text) : StaticTextLayer(w, move(text)), drawer(dynamicDraw) {}

		SmartTextLayer(widget * w, const WideString & s, const handle<Font> & font, const Color & color = Color::black, int fontSize = 14) :
			SmartTextLayer(w, unique<Text>(w->graphics(), s, font, color, fontSize)) {}

		virtual ~SmartTextLayer() {}

		virtual handle<WidgetLayer> clone(widget * widget) const override
		{
			return handle<SmartTextLayer>(widget, _text->clone());
		}

	protected:
		using drawer_t = void(*)(SmartTextLayer * layer, widget * w);

		static api(ui) void dynamicDraw(SmartTextLayer * layer, widget * w);
		static api(ui) void staticDraw(SmartTextLayer * layer, widget * w);

		virtual api(ui) void draw(widget * w) override;
		virtual api(ui) void update() override;

		drawer_t drawer;
		int dynamicCounter;
	};
	
	template<class TextLayerClass, used_t>
	void Text::set(widget * w, const WideString & s, const handle<Font> & font, const Color & color, int fontSize)
	{
		w->components->require<TextComponent>()->set(unique<TextLayerClass>(w, handle<Text>(w->graphics(), s, font, color, fontSize)));
	}
}

//---------------------------------------------------------------------------
#endif