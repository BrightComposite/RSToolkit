//---------------------------------------------------------------------------

#include <ui/Text.h>
#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	void Text::draw(Graphics * graphics, const IntPoint & pos)
	{
		graphics->setColor(_color);
		graphics->bind(_font);
		graphics->setFontSize(_fontSize);

		graphics->draw(_contents, pos);
	}

	TextLayer::TextLayer(Graphics * graphics, const Handle<Text> & text) : _graphics(graphics), _text(text) {}

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

	void TextLayer::draw(Widget * w)
	{
		_text->draw(w->graphics(), w->absPos());
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

	StaticTextLayer::StaticTextLayer(Graphics * graphics, const Handle<Text> & text, const IntSize & size) : 
		TextLayer(graphics, text), _image(nullptr), _surface(_graphics->createSurface(size, _image))
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
		_graphics->bind(_surface);
		auto cc = hold(_graphics->clearColorState(), {1.0f, 1.0f, 1.0f, 0.0f});
		_surface->clear();
		_text->draw(_graphics, {0, 0});
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
