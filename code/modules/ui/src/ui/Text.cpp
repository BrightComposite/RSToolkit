//---------------------------------------------------------------------------

#include <ui/Text.h>
#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace asd
{
	Text::Text(asd::graphics * graphics, const WideString & s, const handle<Font> & font, const Color & color, int fontSize) :
		_graphics(graphics), _contents(s), _font(font), _color(color), _fontSize(fontSize), _size(font->getTextSize(graphics, s, fontSize)) {}

	void Text::draw(const int_point & pos)
	{
		_graphics->setColor(_color);
		_graphics->bind(_font);
		_graphics->setFontSize(_fontSize);

		_graphics->draw(_contents, pos);
	}

	void Text::setDynamic(widget * w, const WideString & s, const handle<Font> & font, const Color & color, int fontSize)
	{
		cast::as<TextComponent>(w)->set(unique<TextLayer>(w, s, font, color, fontSize));
	}

	void Text::setSmart(widget * w, const WideString & s, const handle<Font> & font, const Color & color, int fontSize)
	{
		cast::as<TextComponent>(w)->set(unique<SmartTextLayer>(w, s, font, color, fontSize));
	}

	void Text::setContents(widget * w, const WideString & s)
	{
		auto c = w->components->seek<TextComponent>();

		if(c == nullptr)
			return;

		auto l = c->layer();

		if(l == nullptr)
			return;

		l->setContents(s);
	}

	TextLayer * Text::get(widget * w)
	{
		auto c = w->components->seek<TextComponent>();

		if(c == nullptr)
			return nullptr;

		return c->layer();
	}

	void Text::clear(widget * w)
	{
		w->components->remove<TextComponent>();
	}

	TextLayer::TextLayer(widget * w, unique<Text> && text) : WidgetLayer(2), _graphics(w->graphics()), _text(std::move(text)) {}

	void TextLayer::draw(widget * w)
	{
		_text->draw(w->absPos());
	}

	StaticTextLayer::StaticTextLayer(widget * w, unique<Text> && text) : TextLayer(w, std::move(text))
	{
		update();
	}

	void StaticTextLayer::draw(widget * w)
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
		_surface->addBuffer(_image, image_format::rgba32f);
		_graphics->bind(_surface);
		auto cc = hold(_graphics->clearColorState(), {0.0f, 0.0f, 0.0f, 0.0f});
		_surface->clear();
		_text->draw({0, 0});
		_graphics->bind(old);
	}

	void SmartTextLayer::dynamicDraw(SmartTextLayer * layer, widget * w)
	{
		layer->TextLayer::draw(w);

		if(++layer->dynamicCounter > 3)
		{
			layer->drawer = staticDraw;
			layer->StaticTextLayer::update();
		}
	}

	void SmartTextLayer::staticDraw(SmartTextLayer * layer, widget * w)
	{
		layer->StaticTextLayer::draw(w);
	}

	void SmartTextLayer::draw(widget * w)
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
