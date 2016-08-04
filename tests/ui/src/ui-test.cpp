//---------------------------------------------------------------------------

#include <iostream>
#include <chrono>
#include <random>

#include <application/Starter.h>

#include <core/function/Function.h>
#include <core/function/Thread.h>

#include <graphics/Provider.h>

#include <freeimage/FreeImageConverter.h>

#include <freetype/FreeTypeDecoder.h>

#include <io/FileSystem.h>

#include <math/Vector.h>
#include <math/Matrix.h>

#include <ui/Window.h>
#include <ui/Panel.h>

#include <graphics/font/Font.h>

#include <windows/Message.h>

#include <vld.h>
#include <conio.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	static int render(Handle<Window> & window)
	{
		window->invalidate();
		window->validate();

		return 0;
	}

#define HOTKEY_FULLSCREEN 0x20

	static int load()
	{
		auto graphics = GraphicsProvider::provide();

		graphics->setClearColor(1.0f, 1.0f, 0.8f);

		Handle<Window> window(graphics, 0, 0, 800, 600);

		Handle<BackgroundWidget> back(window);
		back->setName("Background");

		auto w = back->append<Panel>(IntRect(10, 10, 100, 100));

		w->attach([](const Widget * w, const IntRect & r) {
			w->graphics()->setColor<hsv>(0.0f, 1.0f, 1.0f);
			w->graphics()->rectangle(w->absRegion());
		});

		window->setCaption(L"Rapture::UI test");
		window->registerHotkey(HOTKEY_FULLSCREEN, VK_RETURN, MOD_ALT);

		subscribe_on(UISpace, HotkeyMessage, *window)
		{
			auto & window = static_cast<Window &>(dest);

			switch(msg->id)
			{
				case HOTKEY_FULLSCREEN:
					window.toggleFullscreen();
					break;
			}
		};

		subscribe_on(UISpace, KeyUpMessage, *window)
		{
			auto & window = static_cast<Window &>(dest);

			switch(msg->key)
			{
				case VK_ESCAPE:
					window.close();
					break;
			}
		};

		ThreadLoop::add(processWindowMessage);
		ThreadLoop::add(std::bind(render, window));

		window->attachThread();
		window->show();
		window->centralize();

		ThreadLoop::run();

		_getch();
		return 0;
	}

	static Entrance open(load);
}

//---------------------------------------------------------------------------
