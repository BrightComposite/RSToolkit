//---------------------------------------------------------------------------

#include <iostream>
#include <chrono>
#include <random>

#include <application/Starter.h>

#include <core/function/Function.h>
#include <core/function/Thread.h>

#include <meta/XPack.h>

#include <graphics/Provider.h>

#include <freeimage/FreeImageConverter.h>
#include <freetype/FreeTypeDecoder.h>

#include <ui/Text.h>
#include <ui/UIPalette.h>
#include <ui/WidgetLayers.h>
#include <ui/Window.h>

#include <graphics/font/Font.h>

#ifdef WIN32
#include <windows/Message.h>
#include <vld.h>
#endif

//---------------------------------------------------------------------------

namespace asd
{
	static Entrance open([]() {
		FreeTypeDecoder::initialize();
		FreeImageConverter::initialize();

		Display display;
		
		Color backgroundColor(0.2f, 0.2f, 0.2f);

		auto graphics = GraphicsProvider::provide(display);
		graphics->setClearColor(backgroundColor);

		Handle<Window> window(graphics, 0, 0, 800, 600, L"asd::UI test");
		window->setBackgroundColor(backgroundColor);

		StandartUIPalette palette(window);

		auto * w = palette.create("label", window->root());

		with(ColoredButtonDecorator) {
			_.background({0.4f, 0.4f, 0.4f});
			_.pressed({0.1f, 0.1f, 0.1f});
			_.hovered({0.6f, 0.6f, 0.6f});
			_.boundary({0.5f, 0.5f, 0.5f}, 1);
			_.apply(w);
		}

		window->show();
		window->centralize();

		subscription(*window) {
			onmessage(KeyUpMessage) {
				switch(msg->key) {
					/*case VK_ESCAPE:
						dest.close();
						break;*/
				}
			};

			onmessage(WindowCloseMessage) {
				ThreadLoop::stop();
			};
		}

#ifdef WIN32
		ThreadLoop::add(processWindowMessage);
#endif
		
		ThreadLoop::add([&window]() {
			window->mouseUpdate();
		});

/*
		thread th([&window]() {
			subscription(WindowCloseMessage, *window) {
				ThreadLoop::stop();
			};
*/
		ThreadLoop::add([&window]() {
			window->update();
		});
/*
			ThreadLoop::run();
		});
*/
		std::cout << "Run thread loop..." << std::endl;
		
		ThreadLoop::run();

		return 0;
	});
}

//---------------------------------------------------------------------------
