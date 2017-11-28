//---------------------------------------------------------------------------

#include <iostream>
#include <chrono>
#include <random>

#include <application/starter.h>

#include <function/method.h>
#include <thread>

#include <meta/xpack.h>

#include <window/window.h>

#include <text/font.h>
#include <opengl/opengl.h>

#ifdef WIN32
#include <windows/Message.h>
#include <vld.h>
#endif

//---------------------------------------------------------------------------

/**
 * [ ] Inject font and image libraries to graphics
 * [ ] Change global thread loop to event loop
 */

namespace asd
{
	static entrance open([]() {
		gfx::colorf backgroundColor(0.2f, 0.2f, 0.2f);

		opengl::driver driver;

		asd::window w("asd::ui test", {0, 0, 800, 600});
		auto context = w.bind(driver);
		//w.set_background(backgroundColor);
//
//		StandartUIPalette palette(window);
//
//		auto * w = palette.create("label", window->root());
//
//		ColoredButtonDecorator decorator;
//		decorator.background({0.4f, 0.4f, 0.4f});
//		decorator.pressed({0.1f, 0.1f, 0.1f});
//		decorator.hovered({0.6f, 0.6f, 0.6f});
//		decorator.boundary({0.5f, 0.5f, 0.5f}, 1);
//		decorator.apply(w);
//
		w.show();
		w.centralize();
//
//			onmessage(KeyUpMessage) {
//				switch(msg->key) {
//					/*case VK_ESCAPE:
//						dest.close();
//						break;*/
//				}
//			};
//
//			onmessage(WindowCloseMessage) {
//				thread_loop::stop();
//			};

#ifdef WIN32
		thread_loop::add(processWindowMessage);
#endif
//
//		thread_loop::add(make_method(w, mouseUpdate));
//
//
//		thread th([&window]() {
//			subscription(WindowCloseMessage, *window) {
//				thread_loop::stop();
//			};
//
//			thread_loop::add(make_method(w, update));
//
//			thread_loop::run();
//		});

		std::cout << "Run thread loop..." << std::endl;
		
		thread_loop::run();

		return 0;
	});
}

//---------------------------------------------------------------------------
