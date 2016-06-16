//---------------------------------------------------------------------------

#ifndef SCENE_H
#define SCENE_H

//---------------------------------------------------------------------------

#include <ui/UISpace.h>
#include <graphics/Graphics3D.h>

#include <chrono>

#include "Camera.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Scene;
	class SceneObject;
	class Drawable;
	class DrawableObject;

	using namespace std::chrono;
	using namespace std::chrono_literals;

	using ticks_t = long long;

	link_class(scene, Scene, Class<Object>);
	link_class(scene, SceneObject, Class<Object>);
	link_class(scene, Drawable, Class<>);
	link_class(scene, DrawableObject, Class<SceneObject, Drawable>);

	class SceneObject : public Object
	{
		friend class Scene;

		deny_copy(SceneObject);

	public:
		api(scene) SceneObject(Scene * scene, const DoubleVector & pos = DoubleVector::positiveW);
		virtual ~SceneObject() {}

		Scene * scene() const
		{
			return _scene;
		}

		const DoubleVector & pos() const
		{
			return _pos;
		}

		virtual void setPos(const DoubleVector & pos)
		{
			_pos = pos;
		}

	protected:
		virtual void update(ticks_t ticks) {}

		DoubleTranslation _pos;
		Scene * _scene;
	};

	class Drawable
	{
		friend class Scene;

		deny_copy(Drawable);

	public:
		api(scene) Drawable(Scene * scene);
		virtual ~Drawable() {}

	protected:
		virtual void draw(Graphics3D & graphics, const IntRect & viewport, float zoom) const = 0;
	};

	class DrawableObject : public SceneObject, public Drawable
	{
		deny_copy(DrawableObject);

	public:
		DrawableObject(Scene * scene) : SceneObject(scene), Drawable(scene)
		{
			setclass(DrawableObject);
		}

		virtual ~DrawableObject() {}
	};

	class Scene : public Object, public Named
	{
		friend class Drawable;
		friend class SceneObject;
		friend class Widget;

		deny_copy(Scene);

	protected:
		class SceneLayer : public Layer
		{
		public:
			SceneLayer(Widget * widget, Scene * scene) : Layer(widget, INT_MAX), _scene(scene) {}

			virtual void draw(const IntRect & clipRegion) const override
			{
				_scene->draw(_scene->graphics(), scaleToSquare(_widget->absRegion()));
			}

		protected:
			Scene * _scene;
		};

		typedef std::chrono::high_resolution_clock clock;
		typedef time_point<clock> time_marker;

	public:
		enum class ProjectionMode
		{
			Ortho,
			Perspective
		};

		Scene(Widget * widget, const string & name = "unknown scene") : Scene(name)
		{
			if(widget->graphics() notkindof (Graphics3D))
				throw Exception("Widget for the scene must support 3D graphics!");

			_widget = widget;
			_widget->append<SceneLayer>(this);
			connect(*_widget, this, &Scene::onWidgetResize);

			_firstTick = _lastTick = clock::now();
			_ticks = 0;

			setOrtho();
		}

		virtual ~Scene()
		{
			disconnect(*_widget, this, &Scene::onWidgetResize);

			for(auto & obj : _objects)
				obj->_scene = nullptr;
		}

		Graphics3D & graphics() const
		{
			return *static_cast<Graphics3D *>(_widget->graphics());
		}

		Widget & widget() const
		{
			return *_widget;
		}

		UISpace & space() const
		{
			return *_widget->space();
		}

		void invalidate() const
		{
			space().invalidate(_widget);
		}

		void render() const
		{
			space().invalidate(_widget);
			space().validate();
		}

		void api(scene) attach(Handle<SceneObject> obj);
		void api(scene) detach(SceneObject * obj);

		void api(scene) setOrtho();
		void api(scene) setPerspective();
		void api(scene) setZoom(float zoom);
		void api(scene) setFieldOfView(float fov);

		void api(scene) setTickLength(milliseconds length);
		void api(scene) update();

	protected:
		Scene(const string & name) : Named(name)
		{
			setclass(Scene);
		}

		void draw(Graphics3D & graphics, const IntRect & viewport) const
		{
			for(auto & drawable : _drawables)
				drawable->draw(graphics, viewport, _zoom);
		}

		void api(scene) onWidgetResize(Handle<WidgetResizeMessage> & msg, Widget & w);
		void api(scene) setProjection(fmat && projection);

		Widget * _widget;
		Array<SceneObject> _objects;
		array_list<Drawable *> _drawables;

		float _zoom = 0.01f;
		float _fov = 90.0f;
		ProjectionMode _projectionMode = ProjectionMode::Ortho;

		milliseconds _tickLength = 1ms;
		time_marker _lastTick;
		time_marker _firstTick;

		ticks_t _ticks = 0;
	};

	class SceneProvider
	{

		Map<string, Scene> _scenes;
	};
}

//---------------------------------------------------------------------------
#endif
