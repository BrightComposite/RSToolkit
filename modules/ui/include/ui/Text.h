//---------------------------------------------------------------------------

#ifndef TEXT_H
#define TEXT_H

//---------------------------------------------------------------------------

#include <ui/Widget.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Text : public Layer
	{
	public:
		enum Type
		{
			Static,
			Dynamic
		};

		Text(Widget * w, const WideString & contents) : Layer(w), _graphics(w->graphics()), _contents(contents) {}
		virtual ~Text() {}

		const WideString & contents() const
		{
			return _contents;
		}

		void setContents(const WideString & contents)
		{
			_contents = contents;
			update();
		}

		api(ui) void attach(Widget * w);

		static api(ui) Text * get(Widget * w);
		static api(ui) void clear(Widget * w);

	protected:
		virtual void update() = 0;

		Graphics * _graphics;
		WideString _contents;
	};

	class TextComponent : public WidgetComponent
	{

	};

	class StaticText : public Text
	{
	public:
		StaticText(Widget * w, const WideString & contents) : Text(w, contents), _image(nullptr), _surface(w->graphics()->createSurface(w->size(), _image)) {}
		virtual ~StaticText() {}

	protected:
		virtual void draw() const override
		{
			_graphics->draw(_image, 0, 0);
		}

		virtual void update() override
		{
			auto old = _graphics->surface();
			_graphics->bind(_surface);
			_graphics->draw(_contents, 0, 0);
			_graphics->bind(old);
		}

		Handle<Image> _image;
		Handle<Surface> _surface;
	};

	class DynamicText : public Text
	{
	public:
		DynamicText(Widget * w, const WideString & contents) : Text(w, contents) {}
		virtual ~DynamicText() {}

		const WideString & contents() const
		{
			return _contents;
		}

		virtual void setContents(const WideString & contents)
		{
			_contents = contents;
		}

	protected:
		virtual void draw() const override
		{
			_graphics->draw(_contents, 0, 0);
		}

		virtual void update() override {}
	};
}

//---------------------------------------------------------------------------
#endif