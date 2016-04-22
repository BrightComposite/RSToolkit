//---------------------------------------------------------------------------

#include <application/Application.h>
#include <ui/Window.h>
#include <ui/Panel.h>

#include <opengl/OpenGL3_3.h>
#include <windows/Message.h>

#include <freetype/FreeTypeDecoder.h>

#include <art/object/Snake.h>
#include <utils/FpsCounter.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	static int load()
	{
		FreeTypeDecoder::initialize();

		ShowWindow(GetConsoleWindow(), SW_HIDE);

		Graphics3D * graphics = GLGraphics::initialize();
		FinalAction finally(GLGraphics::free);

		graphics->setClearColor({0.0f, 0.0f, 0.0f});

		Handle<Window> window(graphics, 0, 0, 1024, 758);
		Handle<WindowBackground> back(window);
		back->name = "Background";

		Handle<Panel> frame(back);
		frame->setPlacement({0.5f, 0.5f, 0.5f, 0.5f}, {-300, -300, 300, 300});
		frame << [](const Widget * w, const IntRect & clipRect)
		{
			w->graphics()->setColor({0.25f, 0.25f, 0.25f});
			w->graphics()->rectangle(w->absRegion());
		};

		Handle<Panel> frame0(frame);
		frame0->setPlacement(ModelMask::FullSize, {4, 4, -4, -4});
		frame0 << [](const Widget * w, const IntRect & clipRect)
		{
			w->graphics()->setColor({0.5f, 0.5f, 0.5f});
			w->graphics()->rectangle(w->absRegion());
		};

		Handle<Panel> frame1(frame0);
		frame1->setPlacement(ModelMask::FullSize, {3, 3, -3, -3});
		frame1 << [](const Widget * w, const IntRect & clipRect)
		{
			w->graphics()->setColor({0.75f, 0.75f, 0.75f});
			w->graphics()->rectangle(w->absRegion());
		};

		Handle<Panel> frame2(frame1);
		frame2->setPlacement(ModelMask::FullSize, {2, 2, -2, -2});
		frame2 << [](const Widget * w, const IntRect & clipRect)
		{
			w->graphics()->setColor({1.0f, 1.0f, 1.0f});
			w->graphics()->rectangle(w->absRegion());
		};

		Handle<Panel> panel(frame2);
		panel->setPlacement(ModelMask::FullSize, {1, 1, -1, -1});
		panel << [](const Widget * w, const IntRect & clipRect)
		{
			w->graphics()->setColor({0.0f, 0.0f, 0.0f});
			w->graphics()->rectangle(w->absRegion());
		};

		auto arial = Font::load("arial.ttf");

		graphics->bind(arial);
		graphics->setFontSize(16);

		Handle<String> project_name("Independence");
		Handle<Panel> project_name_panel(back);
		project_name_panel->setPlacement(10, 10, 0, 0);
		project_name_panel->setSize(graphics->getTextSize(*project_name));

		project_name_panel << [arial, project_name](const Widget * w, const IntRect & clipRect)
		{
			auto * graphics = w->graphics();
			auto & region = w->absRegion();
			graphics->bind(arial);
			graphics->setFontSize(16);

			graphics->setColor({1.0f, 1.0f, 1.0f});
			graphics->draw(*project_name, region.left, region.top);
		};

		Handle<String> text("Press Esc to quit");
		Handle<Panel> text_panel(back);
		text_panel->setPlacement(ModelMask::RightTop, {0, 10, -10, 0});
		text_panel->setSize(graphics->getTextSize(*text));

		text_panel << [arial, text](const Widget * w, const IntRect & clipRect)
		{
			auto * graphics = w->graphics();
			auto & region = w->absRegion();
			graphics->bind(arial);
			graphics->setFontSize(16);

			graphics->setColor({1.0f, 1.0f, 1.0f});
			graphics->draw(*text, region.left, region.top);
		};

		Handle<FpsCounter> counter(emptiness);

		Handle<Panel> fps_panel(back);
		fps_panel->setPlacement({0.5f, 1.0f, 0.5f, 1.0f}, {-40, -30, 40, -10});
		fps_panel << [counter, arial](const Widget * w, const IntRect & clipRect)
		{
			auto * graphics = w->graphics();
			auto & region = w->absRegion();
			graphics->bind(arial);
			graphics->setFontSize(16);

			auto text = String::assemble(counter->get(), " fps");
			auto textSize = graphics->getTextSize(text);
			int left = region.left + (region.width() - textSize.x) / 2;
			int top = region.top + (region.height() - textSize.y) / 2;

			graphics->setColor({1.0f, 1.0f, 1.0f});
			graphics->draw(text, left, top);
		};
		
		Handle<Scene> scene(panel);

		array<color3, 7> colors {
			color3{1.0f, 0.0f, 0.0f},
			color3{1.0f, 0.5f, 0.0f},
			color3{1.0f, 1.0f, 0.0f},
			color3{0.0f, 1.0f, 0.0f},
			color3{0.0f, 1.0f, 1.0f},
			color3{0.0f, 0.0f, 1.0f},
			color3{0.5f, 0.0f, 1.0f}
		};

		for(auto & c : colors)
			scene->append<Snake>(c);

		dest_connect(*window, WindowAdapter, KeyDownMessage)
		{
			switch(msg->key)
			{
				case VK_ESCAPE:
					dest.close();
					break;
			}
		};

		window->setBorderStyle(BorderStyle::Static);
		window->setCaption("Independence");
		window->centralize();
		window->show();

		ThreadLoop::add(processWindowMessage);
		ThreadLoop::add([scene, window, counter, fps_panel] () mutable
		{
			//std::this_thread::sleep_for(1ms);
			
			counter->next();
			window->invalidate(fps_panel);
			
			scene->invalidate();
			window->validate();

			return 0;
		});

		ThreadLoop::run();

		return 0;
	}

	static Entrance open(load);
}

//---------------------------------------------------------------------------
