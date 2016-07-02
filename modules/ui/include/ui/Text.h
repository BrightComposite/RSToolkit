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
	class TextComponent;

	class Text : public Shared
	{
	public:
		Text(const WideString & s, const Handle<Font> & font, const Color & color = Color{0.0f, 0.0f, 0.0f}, int fontSize = 14) :
			_contents(s), _font(font), _color(color), _fontSize(fontSize) {}

		const WideString & contents() const
		{
			return _contents;
		}

		virtual void setContents(const WideString & contents)
		{
			_contents = contents;
		}

		virtual api(ui) void draw(Graphics * graphics, const IntPoint & pos);

		template<class TextLayerClass = TextLayer, class ... A, useif <based_on<TextLayerClass, TextLayer>::value, can_construct<TextLayerClass, Graphics *, A...>::value> endif>
		static void set(Widget * w, A &&... args)
		{
			w->require<TextComponent>()->setLayer(Handle<TextLayerClass>(w->graphics(), forward<A>(args)...));
		}

		static api(ui) TextLayer * get(Widget * w);
		static api(ui) void clear(Widget * w);

	protected:
		WideString _contents;

		Color _color;
		Handle<Font> _font;
		int _fontSize = 14;
	};

	class TextLayer : public WidgetLayer
	{
		friend_handle;

	public:
		api(ui) TextLayer(Graphics * graphics, const Handle<Text> & text);
		virtual ~TextLayer() {}

		const WideString & contents() const
		{
			return _text->contents();
		}

		const Handle<Text> & text() const
		{
			return _text;
		}

		void setContents(const WideString & contents)
		{
			_text->setContents(contents);
			update();
		}

		void setText(const Handle<Text> & contents)
		{
			_text = contents;
			update();
		}

	protected:
		virtual api(ui) void draw(Widget * w) override;
		virtual void update() {}

		Graphics * _graphics;
		Handle<Text> _text;
	};

	class TextComponent : public WidgetComponent
	{
	public:
		api(ui) TextComponent(Widget * widget);
		api(ui) virtual ~TextComponent();

		const Handle<TextLayer> & layer() const
		{
			return _layer;
		}

		api(ui) void setLayer(const Handle<TextLayer> & layer);
		api(ui) void setText(const Handle<Text> & contents);
		api(ui) void setContents(const WideString & text);

	protected:
		Handle<TextLayer> _layer;
	};

	create_component(ui, TextComponent);

	class StaticTextLayer : public TextLayer
	{
	public:
		api(ui) StaticTextLayer(Graphics * graphics, const Handle<Text> & text, const IntSize & size);
		virtual ~StaticTextLayer() {}

		void requestData(ImageData * data)
		{
			_image->requestData(data);
		}

	protected:
		virtual api(ui) void draw(Widget * w) override;
		virtual api(ui) void update() override;

		Handle<Image> _image;
		Handle<Surface> _surface;
	};

	class SmartTextLayer : public StaticTextLayer
	{
	public:
		SmartTextLayer(Graphics * graphics, const Handle<Text> & text, const IntSize & size) : StaticTextLayer(graphics, text, size), drawer(dynamicDraw) {}
		virtual ~SmartTextLayer() {}

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