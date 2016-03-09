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
		Graphics3D * graphics = D3DGraphics::initialize();
		FinalAction finally(D3DGraphics::free);

		Handle<Window> window(graphics, 0, 0, 1024, 768);

		Handle<WindowBackground> back(window);
		back->name = "Background";

		graphics->setClearColor({1.0f, 1.0f, 1.0f});

		window->setCaption(L"Rapture::UI test");

	#define HOTKEY_FULLSCREEN 0x20

		window->registerHotkey(HOTKEY_FULLSCREEN, VK_RETURN, MOD_ALT);

		dest_connect(*window, WindowAdapter, WindowHotkeyMessage)
		{
			auto window = static_cast<Window *>(&dest);

			switch(msg->id)
			{
				case HOTKEY_FULLSCREEN:
					window->toggleFullscreen();
					break;
			}
		};

		dest_connect(*window, WindowAdapter, KeyUpMessage)
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
