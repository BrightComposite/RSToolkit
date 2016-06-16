//---------------------------------------------------------------------------

#include <iostream>
#include <chrono>
#include <random>

#include <application/Application.h>
#include <application/ThreadLoop.h>

#include <core/action/Action.h>
#include <core/action/Thread.h>

#include <direct3d/Direct3D11.h>

#include <freeimage/FreeImageConverter.h>

#include <freetype/FreeTypeDecoder.h>

#include <io/FileSystem.h>

#include <math/Vector.h>
#include <math/Matrix.h>

#include <ui/Window.h>
#include <ui/Panel.h>

#include <graphics/text/Font.h>

#include <windows/Message.h>

#include <vld.h>

#include "matrix-test.h"
#include <application/Starter.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::bind;

	using std::cout;
	using std::endl;
	using std::thread;

	using boost::filesystem::path;

	using namespace std::chrono;
	using namespace DirectX;

	typedef high_resolution_clock hrc;

	static int render(Handle<Window> & window);

	time_point<hrc> lastFrame, lastSecond;

	float foo(float n)
	{
		return n;
	}

	class ChildWidget : public Panel
	{
	public:
		ChildWidget(Widget * parent) : ChildWidget(parent, *parent) {}
		ChildWidget(Widget * parent, const IntRect & region) : Panel(parent, region)
		{
			setFocusability(true);
		}
	};

	static void loadWindow()
	{
		using namespace Rapture;
		using Rapture::Font;

		FreeImageConverter::initialize();
		FreeTypeDecoder::initialize();

		auto graphics = GraphicsProvider::provide();

		{
			Handle<Window> window(graphics, 0, 0, 1280, 480);

			Handle<BackgroundWidget> back(window);
			back->setName("Background");

			graphics->setClearColor(1.0f, 1.0f, 1.0f);

		#define HOTKEY_FULLSCREEN 0x20

			window->registerHotkey(HOTKEY_FULLSCREEN, VK_RETURN, MOD_ALT);

			subscribe_on(UISpace, HotkeyMessage, *window)
			{
				auto window = static_cast<Window *>(&dest);

				switch(msg->id)
				{
					case HOTKEY_FULLSCREEN:
						window->toggleFullscreen();
						break;
				}
			};

			subscribe_on(Widget, MouseDownMessage)
			{
				switch(msg->button)
				{
					case MouseButton::Left:
						cout << "Widget " << dest.name() << " has been pressed!" << endl;
						break;
				}
			};

			subscribe_on(UISpace, KeyUpMessage, *window)
			{
				auto window = static_cast<Window *>(&dest);

				switch(msg->key)
				{
					case VK_ESCAPE:
						window->close();
						break;
				}
			};

			auto root = initial_path();

			if(root.filename() == "x86" || root.filename() == "x64")
			{
				root = root.parent_path().parent_path();
			}
			else
			{
				root = root.parent_path().parent_path().parent_path();
			}

			auto panelDrawer = [](const Widget * widget, const IntRect & region) {
				IntRect inner(widget->absRegion());
				inner.resize(-2);

				auto g = widget->graphics();

				g->setColor(0.4f, 0.4f, 0.4f, 1.0f);
				g->rectangle(widget->absRegion());
				g->setColor(1.0f, 1.0f, 1.0f, 1.0f);
				g->rectangle(inner);
			};

			auto childDrawer = [](const Widget * widget, const IntRect & region) {
				IntRect inner(widget->absRegion());
				inner.resize(-2);

				auto g = widget->graphics();

				if(widget->isFocused())
					g->setColor(0.75f, 0.75f, 0.75f, 1.0f);
				else
					g->setColor(0.0f, 0.0f, 0.0f, 1.0f);

				g->rectangle(widget->absRegion());

				if(widget->isPressed())
					g->setColor(0.0f, 1.0f, 0.0f, 1.0f);
				else if(widget->isPointed())
					g->setColor(1.0f, 1.0f, 0.0f, 1.0f);
				else
					g->setColor(0.5f, 0.5f, 0.5f, 1.0f);

				g->rectangle(inner);
			};

			std::array_list<IntRect> rects = {
				{10, 10, 30, 30},
				{40, 10, 60, 30},
				{10, 40, 30, 60},
				{40, 40, 60, 60}
			};

			const int dx = 10;
			const int dy = 10;
			const int w = 70;
			const int h = 70;

			std::default_random_engine rndm;
			std::bernoulli_distribution dist;
			auto isVisible = std::bind(dist, rndm);

			for(int iy = 0; iy < 4; ++iy)
			{
				for(int ix = 0; ix < 4; ++ix)
				{
					int x = (dx + w) * (ix + 1);
					int y = (dy + h) * (iy + 1) + 100;

					auto panel = back->append<Panel>(IntRect {x - w, y - h, x, y});

					panel->setName("panel("_s << ix << ',' << iy << ')');
					panel->attach(panelDrawer);

					for(int ir = 0; ir < 4; ++ir)
					{
						auto child = panel->append<ChildWidget>(rects[ir]);

						child->setName("child("_s << ix << ',' << iy << ',' << ir << ')');
						child->setVisibility(isVisible());
						child->attach(childDrawer);
					}
				}
			}

			try
			{
				FontCache::set("Times New Roman", {
					{FontStyle::Regular,	Font::load("times.ttf")},
					{FontStyle::Bold,		Font::load("timesbd.ttf")},
					{FontStyle::Italic,		Font::load("timesi.ttf")},
					{FontStyle::BoldItalic, Font::load("timesbi.ttf")}
				});

				FontCache::set("Arial", {
					{FontStyle::Regular,	Font::load("arial.ttf")},
					{FontStyle::Bold,		Font::load("arialbd.ttf")},
					{FontStyle::Italic,		Font::load("ariali.ttf")},
					{FontStyle::BoldItalic, Font::load("arialbi.ttf")}
				});

				FontCache::set("Trebuchet", {
					{FontStyle::Regular,	Font::load("trebuc.ttf")},
					{FontStyle::Bold,		Font::load("trebucbd.ttf")},
					{FontStyle::Italic,		Font::load("trebucit.ttf")},
					{FontStyle::BoldItalic, Font::load("trebucbi.ttf")}
				});

				FontCache::set("Pristina", Font::load("pristina.ttf"));
			}
			catch(...) {}

			auto arial_italic = FontCache::get("Arial", FontStyle::Italic);
			auto pristina_font = FontCache::get("Pristina", FontStyle::Regular);

			int fontSize = 24;
			Handle<Panel> label(back);
			Handle<Panel> english_label(back);

			Handle<String> engine_text(L"Rapture State Engine");
			Handle<String> english_text(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,-:;\" \'.?!\\/~`");

			graphics->bind(pristina_font);
			graphics->setFontSize(fontSize);

			label->setName("Engine Label");
			label->setPlacement(ModelMask::RightTop, {-10, 10}, graphics->getTextSize(*engine_text));

			english_label->setName("Text Label");
			english_label->setPlacement({10, 10}, graphics->getTextSize(*english_text));

			*label << [engine_text, pristina_font, fontSize](const Widget * widget, const IntRect & region) {
				auto * graphics = widget->graphics();

				graphics->bind(pristina_font);
				graphics->setFontSize(fontSize);

				graphics->setColor(0.8f, 0.8f, 0.86f);
				graphics->draw(*engine_text, widget->absPos());
			};

			*english_label << [english_text, pristina_font, fontSize](const Widget * widget, const IntRect & region) {
				auto * graphics = widget->graphics();

				graphics->bind(pristina_font);
				graphics->setFontSize(fontSize);

				graphics->setColor<hsv>(0.5f, 0.5f, 0.5f);
				graphics->draw(*english_text, widget->absPos());
			};

			fontSize = 16;
			Handle<Panel> russian_label(back);
			Handle<WideString> russian_text(L"ÀÁÂÃÄÅ¨ÆÇÈÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäå¸æçèêëìíîïðñòóôõö÷øùúûüýþÿ");

			graphics->bind(arial_italic);
			graphics->setFontSize(fontSize);

			russian_label->setName("Russian Text Label");
			russian_label->setPlacement({10, 50}, graphics->getTextSize(*russian_text));

			*russian_label << [russian_text, arial_italic, fontSize](const Widget * widget, const IntRect & region) {
				auto * graphics = widget->graphics();

				graphics->bind(arial_italic);
				graphics->setFontSize(fontSize);

				graphics->setColor(0.0f, 0.0f, 0.0f);
				graphics->draw(*russian_text, widget->absPos());
			};

			window->setCaption(L"Rapture::Direct3D test");

			ThreadLoop::add(processWindowMessage);
			ThreadLoop::add(bind(render, window));

			window->show();
			window->centralize();

			lastFrame = lastSecond = hrc::now();
			ThreadLoop::run();

			FontCache::clear();
		}
	}

	static int load()
	{
		matrixTest();
		Handle<Thread> th(loadWindow);
		return 0;
	}

	static Entrance open(load);

	static int render(Handle<Window> & window)
	{
		//std::this_thread::sleep_for(1ms);

		static int frames = 0;
		static int maxframes = 0;

		if(hrc::now() - lastSecond >= 1s)
		{
			if(frames > maxframes)
				maxframes = frames;

			cout << frames << " / " << maxframes << endl;
			frames = 0;
			lastSecond = hrc::now();
		}

		window->invalidate();
		window->validate();
		++frames;

		return 0;
	}
}
