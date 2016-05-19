//---------------------------------------------------------------------------

#ifndef GAME_H
#define GAME_H

//---------------------------------------------------------------------------

#include <application/Application.h>

#include <core/container/Queue.h>

#include <ui/Window.h>
#include <ui/Panel.h>

#include <scene/Scene.h>

#include <graphics/image/io/ImageIO.h>

#include <graphics/Provider.h>

#include <windows/Message.h>

#include <freeimage/FreeImageConverter.h>
#include <freetype/FreeTypeDecoder.h>

#include <game/object/PlayerObject.h>
#include <game/object/BigEyeObject.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	declare_and_link(SimpleGroundObject, Class<PlaneObject, Drawable>);

	class SimpleGroundObject : public PlaneObject, public Drawable
	{
	public:
		SimpleGroundObject(Scene * scene, PhysicalWorld * world, double level) :
			PlaneObject(scene, world, level),
			Drawable(scene)
		{
			setclass(SimpleGroundObject);
		}

		virtual ~SimpleGroundObject() {}

	protected:
		virtual void draw(Graphics3D * graphics, const IntRect & viewport, float zoom) const override
		{
			auto dt = hold(graphics->depthTestModeState(), true);
			graphics->setDepth(0.0f);

			graphics->setColor(0.1f, 0.1f, 0.1f);
			graphics->rectangle(SqRect{-1.0f, -1.0f, 1.0f, (_pos.y - 0.2f) * zoom});
			graphics->setColor(1.0f, 1.0f, 1.0f);
			graphics->rectangle(SqRect{-1.0f, (_pos.y - 0.2f) * zoom, 1.0f, _pos.y * zoom});
		}
	};

	class Game : public Singleton<Game>
	{
	public:
		static int start()
		{
			return instance().load("In The Moonlight");
		}

		static const auto & name()
		{
			return instance()._name;
		}

	protected:
		int load(const string & name)
		{
			this->_name = name;

			FreeImageConverter::initialize();
			FreeTypeDecoder::initialize();

			auto broadphase = unique_handle<btDbvtBroadphase>();
			auto collisionConfiguration = unique_handle<btDefaultCollisionConfiguration>();
			auto dispatcher = unique_handle<btCollisionDispatcher>(collisionConfiguration);

			auto solver = unique_handle<btSequentialImpulseConstraintSolver>();
			auto world = unique_handle<PhysicalWorld>(dispatcher, broadphase, solver, collisionConfiguration);

			world->setGravity({0.0, -9.8, 0.0});

			auto graphics = GraphicsProvider::provide();
			graphics->setClearColor(0.0f, 0.0f, 0.0f);

			UniqueHandle<Window>     window(graphics, 0, 0, 1024, 758);
			Handle<BackgroundWidget> back(window);
			UniqueHandle<Scene>      scene(back);

			scene->setZoom(0.02f);

			//_scene->append<BigEyeObject>();
						
			UniqueHandle<PlayerObject>       player(scene, world, dvec {0.0, 0.0, 0.0, 1.0});
			UniqueHandle<SimpleGroundObject> ground(scene, world, -20);
			UniqueHandle<PlayerController>   controller(player);
						
			connect(*window, onWindowKeyDown);
			connect(*window, onWindowKeyUp);

			window->setCaption(_name);
			window->centralize();
			window->show();

			hideConsole();

			ThreadLoop::add(processWindowMessage);
			ThreadLoop::add(std::bind(mainLoop, scene.inner(), world.inner()));
			ThreadLoop::run();

			return 0;
		}

		static int mainLoop(Scene * scene, PhysicalWorld * world)
		{
			auto & that = instance();

			scene->update();
			world->stepSimulation(1 / 480.0f, 10);
			scene->render();

			return 0;
		}

		static void hideConsole()
		{
			ShowWindow(GetConsoleWindow(), SW_HIDE);
		}

		static void makeScreenshot(Graphics * graphics)
		{
			ImageIO::save(initial_path() / "screenshot.bmp", graphics->requestSurfaceData());
		}

		static void onWindowKeyDown(Handle<KeyDownMessage> & message, UISpace & dest)
		{
			switch(message->key)
			{
				case VK_ESCAPE:
					static_cast<Window &>(dest).close();
					break;
			}
		}

		static void onWindowKeyUp(Handle<KeyUpMessage> & message, UISpace & dest)
		{
			switch(message->key)
			{
				case VK_SNAPSHOT:
					makeScreenshot(dest.graphics());
					break;
			}
		}

		string _name;
	};
}

//---------------------------------------------------------------------------
#endif
