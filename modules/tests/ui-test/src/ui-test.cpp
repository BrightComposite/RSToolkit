//---------------------------------------------------------------------------

#include <iostream>
#include <chrono>
#include <random>

#include <application/Application.h>
#include <application/ThreadLoop.h>

#include <core/action/Action.h>
#include <core/action/Thread.h>

#include <graphics/Config.h>
#include GRAPHICS_INCLUDE

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

//---------------------------------------------------------------------------

namespace Rapture
{
	static int render(Handle<Window> & window)
	{
		//window->invalidate();
		window->validate();

		return 0;
	}

	static int load()
	{
		auto graphics = GraphicsProvider::provide();

		Handle<Window> window(graphics, 0, 0, 1024, 768);

		Handle<BackgroundWidget> back(window);
		back->setName("Background");

		graphics->setClearColor({1.0f, 1.0f, 1.0f});

		window->setCaption(L"Rapture::UI test");

	#define HOTKEY_FULLSCREEN 0x20

		window->registerHotkey(HOTKEY_FULLSCREEN, VK_RETURN, MOD_ALT);

		dest_connect(*window, UISpace, HotkeyMessage)
		{
			auto window = static_cast<Window *>(&dest);

			switch(msg->id)
			{
				case HOTKEY_FULLSCREEN:
					window->toggleFullscreen();
					break;
			}
		};

		dest_connect(*window, UISpace, KeyUpMessage)
		{
			auto window = static_cast<Window *>(&dest);

			switch(msg->key)
			{
				case VK_ESCAPE:
					window->close();
					break;
			}
		};

		ThreadLoop::add(processWindowMessage);
		ThreadLoop::add(std::bind(render, window));

		window->show();
		window->centralize();

		ThreadLoop::run();

		return 0;
	}

	static Entrance open(load);
}

//---------------------------------------------------------------------------
