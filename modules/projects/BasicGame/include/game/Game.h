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

//---------------------------------------------------------------------------

namespace Rapture
{
	class SimpleGroundObject;

	link_class(basicgame, SimpleGroundObject, Class<PlaneObject, Drawable>);

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
		virtual void draw(Graphics3D & graphics, const IntRect & viewport, float zoom) const override
		{
			/*auto dt = hold(graphics.depthTestModeState(), true);
			graphics.setDepth(0.0f);

			graphics.setColor(0.1f, 0.1f, 0.1f);
			graphics.rectangle(SqRect{-1.0f, -1.0f, 1.0f, (_pos.y - 0.2f) * zoom});
			graphics.setColor(1.0f, 1.0f, 1.0f);
			graphics.rectangle(SqRect{-1.0f, (_pos.y - 0.2f) * zoom, 1.0f, _pos.y * zoom});*/
		}
	};

	class CubeObject : public SceneObject, public Drawable
	{
	public:
		CubeObject(Scene * scene, const dvec & pos) : SceneObject(scene, pos), Drawable(scene)
		{
			auto & g = scene->graphics();
			_model.init(&g, g.meshes.colorcube, g.techniques3d.multicolor);
		}

	protected:
		virtual void draw(Graphics3D & graphics, const IntRect & viewport, float zoom) const override
		{
			graphics.setDepth(0.0f);
			auto & v = graphics.viewport();

			FloatTranslation t {_pos};

			_model->setTransform({t, _rot});
			_model->draw();

			auto fm = hold(graphics.fillModeState(), FillMode::Wireframe);
			_model->draw();
		}

		virtual void update(ticks_t ticks) override
		{
			_rot.rotateY(fmath::dtor(0.014f));
			_rot.rotateX(fmath::dtor(0.02f));
			_model->setTransform({fvec(_pos), _rot, _scale});
		}

		fquat _rot;
		fvec _scale;
		Handle<GraphicModel> _model;
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

			btDbvtBroadphase broadphase;
			btDefaultCollisionConfiguration collisionConfiguration;
			btCollisionDispatcher dispatcher(&collisionConfiguration);

			btSequentialImpulseConstraintSolver solver;
			PhysicalWorld world(&dispatcher, &broadphase, &solver, &collisionConfiguration);

			world.setGravity({0.0, -9.8, 0.0});

			auto graphics = GraphicsProvider::provide();
			graphics->setClearColor(1.0f, 1.0f, 1.0f);

			Window window(graphics, 0, 0, 1024, 758);
			Scene scene(Handle<BackgroundWidget>{&window});

			scene.setZoom(0.1f);
			scene.setFieldOfView(90.0f);
			scene.setOrtho();

			Handle<PlayerObject>       player(&scene);
			Handle<CubeObject>		   cube(&scene, dvec {0.0, 0.0, 4.0});
			Handle<SimpleGroundObject> ground(&scene, &world, 0);
			Handle<PlayerController>   controller(player);

			connect(window, onWindowKeyDown);
			connect(window, onWindowKeyUp);

			window.setCaption(_name);
			window.centralize();
			window.show();

			ThreadLoop::add(processWindowMessage);
			ThreadLoop::add(std::bind(mainLoop, std::ref(scene), std::ref(world)));
			ThreadLoop::run();

			return 0;
		}

		static int mainLoop(Scene & scene, PhysicalWorld & world)
		{
			scene.update();
			world.stepSimulation(1 / 4800.0f, 10);
			scene.render();

			return 0;
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

	template struct api(basicgame) Singleton<Game>;
}

//---------------------------------------------------------------------------
#endif
