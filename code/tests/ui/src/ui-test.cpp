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

/**
 * [ ] Create graphics and windows from Display
 * [ ] Inject font and image libraries to graphics
 * [ ] Change global thread loop to event loop
 */

namespace asd
{
	static Entrance open([]() {
		FreeTypeDecoder::initialize(); //! GLOBAL STATE
		FreeImageConverter::initialize(); //! GLOBAL STATE

		Color backgroundColor(0.2f, 0.2f, 0.2f);

		auto graphics = GraphicsProvider::provide(); /// Create graphics from Display
		graphics->setClearColor(backgroundColor);

		Handle<Window> window(graphics, 0, 0, 800, 600, "asd::ui test"); /// MAKE FACTORY?
		window->setBackgroundColor(backgroundColor);

		StandartUIPalette palette(window);

		auto * w = palette.create("label", window->root());

		ColoredButtonDecorator decorator;
		decorator.background({0.4f, 0.4f, 0.4f});
		decorator.pressed({0.1f, 0.1f, 0.1f});
		decorator.hovered({0.6f, 0.6f, 0.6f});
		decorator.boundary({0.5f, 0.5f, 0.5f}, 1);
		decorator.apply(w);

		window->show();
		window->centralize();

		subscription(*window) {
//			onmessage(KeyUpMessage) {
//				switch(msg->key) {
//					/*case VK_ESCAPE:
//						dest.close();
//						break;*/
//				}
//			};

			onmessage(WindowCloseMessage) {
				ThreadLoop::stop();
			};
		}

#ifdef WIN32
		ThreadLoop::add(processWindowMessage);
#endif
		
		ThreadLoop::add(make_method(window, mouseUpdate));

/*
		thread th([&window]() {
			subscription(WindowCloseMessage, *window) {
				ThreadLoop::stop();
			};
*/
		ThreadLoop::add(make_method(window, update));
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
