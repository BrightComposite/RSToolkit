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

#include <ui/Text.h>
#include <ui/UIPalette.h>
#include <ui/WidgetLayers.h>
#include <ui/Window.h>

#include <graphics/font/Font.h>

#include <windows/Message.h>

#include <vld.h>
#include <conio.h>

//---------------------------------------------------------------------------

namespace asd
{
	static int render(Handle<Window> & window)
	{
		window->mouseUpdate();
		window->invalidate();
		window->validate();

		return 0;
	}

#define HOTKEY_FULLSCREEN 0x20

	static int load()
	{
		FreeTypeDecoder::initialize();
		FreeImageConverter::initialize();

		auto graphics = GraphicsProvider::provide();

		Handle<Window> window(graphics, 0, 0, 800, 600);

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

		Unique<StandartUIPalette> palette(window);

		auto w = palette->create("label", window->root());
		w->setPlacement(ModelMask::RightTop, -20, 20, 120, 80);

		ColoredButtonDecorator decorator;

		decorator
			.background({0.4f, 0.4f, 0.4f})
			.pressed({0.1f, 0.1f, 0.1f})
			.hovered({0.6f, 0.6f, 0.6f})
			.boundary({0.5f, 0.5f, 0.5f}, 1)
			.decorate(w);

		Text::setContents(w, "Hello!");

		Color backgroundColor(0.2f, 0.2f, 0.2f);

		graphics->setClearColor(backgroundColor);
		window->setBackgroundColor(backgroundColor);

		window->setCaption("asd::UI test");

		window->attachThread();
		window->show();
		window->centralize();

		ThreadLoop::run();

		return 0;
	}

	static Entrance open(load);
}

//---------------------------------------------------------------------------
