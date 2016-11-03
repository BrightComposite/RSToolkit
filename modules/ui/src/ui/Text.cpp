//---------------------------------------------------------------------------

#include <ui/Text.h>
#include <graphics/Graphics.h>

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
		w->components->require<TextComponent>()->set(Unique<TextLayer>(w, s, font, color, fontSize));
	}

	void Text::setSmart(Widget * w, const WideString & s, const Handle<Font> & font, const Color & color, int fontSize)
	{
		w->components->require<TextComponent>()->set(Unique<SmartTextLayer>(w, s, font, color, fontSize));
	}

	void Text::setContents(Widget * w, const WideString & s)
	{
		auto c = w->components->seek<TextComponent>();

		if(c == nullptr)
			return;

		auto l = c->layer();

		if(l == nullptr)
			return;

		l->setContents(s);
	}

	TextLayer * Text::get(Widget * w)
	{
		auto c = w->components->seek<TextComponent>();

		if(c == nullptr)
			return nullptr;

		return c->layer();
	}

	void Text::clear(Widget * w)
	{
		w->components->remove<TextComponent>();
	}

	TextLayer::TextLayer(Widget * w, Unique<Text> && text) : WidgetLayer(2), _graphics(w->graphics()), _text(move(text)) {}

	void TextLayer::draw(Widget * w)
	{
		_text->draw(w->absPos());
	}

	StaticTextLayer::StaticTextLayer(Widget * w, Unique<Text> && text) : TextLayer(w, move(text))
	{
		update();
	}

	void StaticTextLayer::draw(Widget * w)
	{
		if(_image != nullptr)
			_graphics->draw(_image, w->absPos());
	}

	void StaticTextLayer::update()
	{
		auto & size = _text->size();

		if(size.x * size.y == 0)
		{
			_image = nullptr;
			return;
		}

		auto old = _graphics->surface();
		_surface = _graphics->createSurface(size);
		_surface->addBuffer(_image, ImageFormat::rgba32f);
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
