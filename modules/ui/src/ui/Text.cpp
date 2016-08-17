//---------------------------------------------------------------------------

#include <ui/Text.h>
#include <graphics/Graphics3D.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	Text::Text(Graphics * graphics, const WideString & s, const Handle<Font> & font, const Color & color, int fontSize) :
		_graphics(graphics), _contents(s), _font(font), _color(color), _fontSize(fontSize), _size(font->getTextSize(graphics, s, fontSize)) {}

	void Text::draw(const IntPoint & pos)
	{
		_graphics->setColor(_color);
		_graphics->bind(_font);
		_graphics->setFontSize(_fontSize);

		_graphics->draw(_contents, pos);
	}

	void Text::setDynamic(Widget * w, const WideString & s, const Handle<Font> & font, const Color & color, int fontSize)
	{
		w->require<TextComponent>()->setLayer(Handle<TextLayer>(w, Handle<Text>(w->graphics(), s, font, color, fontSize)));
	}

	void Text::setSmart(Widget * w, const WideString & s, const Handle<Font> & font, const Color & color, int fontSize)
	{
		w->require<TextComponent>()->setLayer(Handle<SmartTextLayer>(w, Handle<Text>(w->graphics(), s, font, color, fontSize)));
	}

	void Text::setContents(Widget * w, const WideString & s)
	{
		auto c = w->seek<TextComponent>();

		if(c == nullptr)
			return;

		c->layer()->setContents(s);
	}

	TextLayer * Text::get(Widget * w)
	{
		auto c = w->seek<TextComponent>();

		if(c == nullptr)
			return nullptr;

		return c->layer();
	}

	void Text::clear(Widget * w)
	{
		w->detach<TextComponent>();
	}

	TextLayer::TextLayer(Widget * w, const Handle<Text> & text) : _graphics(w->graphics()), _text(text) {}

	void TextLayer::draw(Widget * w)
	{
		_text->draw(w->absPos());
	}

	TextComponent::TextComponent(Widget * widget) : WidgetComponent(widget), _layer(nullptr) {}

	TextComponent::~TextComponent()
	{
		if(_layer != nullptr)
			_widget->detach(_layer);
	}

	void TextComponent::setContents(const WideString & text)
	{
		if(_layer == nullptr)
			return;

		_layer->setContents(text);
	}

	void TextComponent::setText(const Handle<Text> & text)
	{
		if(_layer == nullptr)
			return;

		_layer->setText(text);
	}

	void TextComponent::setLayer(const Handle<TextLayer> & layer)
	{
		if(_layer != nullptr)
			_widget->detach(_layer);

		_layer = layer;
		_widget->attach(_layer);
	}

	StaticTextLayer::StaticTextLayer(Widget * w, const Handle<Text> & text) : 
		TextLayer(w, text)
	{
		update();
	}

	void StaticTextLayer::draw(Widget * w)
	{
		_graphics->draw(_image, w->absPos());
	}

	void StaticTextLayer::update()
	{
		auto old = _graphics->surface();
		_surface = _graphics->createSurface(_text->size());
		_surface->addBuffer(_image);
		_graphics->bind(_surface);
		auto cc = hold(_graphics->clearColorState(), {0.0f, 0.0f, 0.0f, 0.0f});
		_surface->clear();
		_text->draw({0, 0});
		_graphics->bind(old);
	}

	void SmartTextLayer::dynamicDraw(SmartTextLayer * layer, Widget * w)
	{
		layer->TextLayer::draw(w);

		if(++layer->dynamicCounter > 3)
		{
			layer->drawer = staticDraw;
			layer->StaticTextLayer::update();
		}
	}

	void SmartTextLayer::staticDraw(SmartTextLayer * layer, Widget * w)
	{
		layer->StaticTextLayer::draw(w);
	}

	void SmartTextLayer::draw(Widget * w)
	{
		drawer(this, w);
	}

	void SmartTextLayer::update()
	{
		dynamicCounter = 0;
		drawer = dynamicDraw;
	}
}

//---------------------------------------------------------------------------
