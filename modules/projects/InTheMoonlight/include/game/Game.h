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

#include <direct3d/Direct3D11.h>
#include <windows/Message.h>

#include <freeimage/FreeImageConverter.h>
#include <freetype/FreeTypeDecoder.h>

#include <game/object/PlayerObject.h>
#include <game/object/BigEyeObject.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	link_class(SimpleGroundObject, MetaClass<PlaneObject, Drawable>);

	class SimpleGroundObject : public PlaneObject, public Drawable
	{
	public:
		SimpleGroundObject(Scene * scene, PhysicalWorld * world, double level) :
			PlaneObject(
				scene,
				world,
				level
				),
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

			graphics->setColor({0.1f, 0.1f, 0.1f});
			graphics->rectangle(SqRect{-1.0f, -1.0f, 1.0f, (_level - 0.1f) * zoom});
			graphics->setColor({1.0f, 1.0f, 1.0f});
			graphics->rectangle(SqRect{-1.0f, (_level - 0.1f) * zoom, 1.0f, _level * zoom});
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

		static auto & scene()
		{
			return instance()._scene;
		}

		static auto & world()
		{
			return instance()._world;
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

			this->_world = world;
			this->_world->setGravity({0.0, -9.8, 0.0});

			Graphics3D * graphics = D3DGraphics::initialize();
			graphics->setClearColor({0.0f, 0.0f, 0.0f});

			{
				auto window = unique_handle<Window>(graphics, 0, 0, 1024, 758);
				this->_window = window;

				Handle<WindowBackground> back(_window);

				{
					auto scene = unique_handle<Scene>(back);
					this->_scene = scene;

					{
						//_scene->append<BigEyeObject>();
						
						this->_player = _scene->append<PlayerObject>(_world);
						this->_ground = _scene->append<SimpleGroundObject>(_world, -20);

						Handle<PlayerController> controller(this->_player);
						
						connect(*this->_window, onWindowKeyDown);
						connect(*this->_window, onWindowKeyUp);

						this->_window->setCaption(_name);
						this->_window->centralize();
						this->_window->show();

						hideConsole();

						ThreadLoop::add(processWindowMessage);
						ThreadLoop::add(mainLoop);
						ThreadLoop::run();
					}
				}
			}

			D3DGraphics::free();

			return 0;
		}

		static int mainLoop()
		{
			auto & that = instance();

			that._scene->update();

			that._world->stepSimulation(1 / 480.f, 10);

			that._scene->invalidate();
			that._window->validate();

			return 0;
		}

		static void hideConsole()
		{
			ShowWindow(GetConsoleWindow(), SW_HIDE);
		}

		static void makeScreenshot(Graphics * graphics)
		{
			ImageIO::save(initial_path() / "screenshot.bmp", graphics->surfaceData());
		}

		static void onWindowKeyDown(Handle<KeyDownMessage> & message, WindowAdapter & dest, const Subject * source)
		{
			switch(message->key)
			{
				case VK_ESCAPE:
					dest.close();
					break;
			}
		}

		static void onWindowKeyUp(Handle<KeyUpMessage> & message, WindowAdapter & dest, const Subject * source)
		{
			switch(message->key)
			{
				case VK_SNAPSHOT:
					makeScreenshot(scene()->graphics());
					break;
			}
		}

		PhysicalWorld * _world;

		SimpleGroundObject * _ground;
		PlayerObject * _player;

		Window * _window;
		Scene * _scene;

		string _name;
	};
}

//---------------------------------------------------------------------------
#endif
