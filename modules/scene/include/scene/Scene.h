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

	link_class(Scene, Class<Object>);
	link_class(SceneObject, Class<Object>);
	link_class(Drawable, Class<>);
	link_class(DrawableObject, Class<SceneObject, Drawable>);

	class SceneObject : public Object
	{
		friend class Scene;

	public:
		SceneObject(Scene * scene, const DoubleVector & pos = DoubleVector::positiveW);
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

	public:
		Drawable(Scene * scene);
		virtual ~Drawable() {}

	protected:
		virtual void draw(Graphics3D * graphics, const IntRect & viewport, float zoom) const = 0;
	};

	class DrawableObject : public SceneObject, public Drawable
	{
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
		Scene(Handle<Widget> widget, const string & name = "unknown") : Scene(name)
		{
			if(widget->graphics() notkindof (Graphics3D))
				throw Exception("Widget for the scene must support 3D graphics!");

			_widget = widget;
			widget->append<SceneLayer>(this);

			_firstTick = _lastTick = clock::now();
			_ticks = 0;

			updateViewport();
		}

		virtual ~Scene()
		{
			for(auto & obj : _objects)
				obj->_scene = nullptr;
		}

		Graphics3D * graphics() const
		{
			return static_cast<Graphics3D *>(_widget->graphics());
		}

		Widget * widget() const
		{
			return _widget;
		}

		UISpace * space() const
		{
			return _widget->space();
		}

		Camera * camera() const
		{
			return _camera;
		}

		void invalidate() const
		{
			space()->invalidate(_widget);
		}

		void render() const
		{
			space()->invalidate(_widget);
			space()->validate();
		}

		void attach(Handle<SceneObject> obj);
		void detach(SceneObject * obj);

		void setProjection(const fmat & projection)
		{
			graphics()->updateUniform<Uniforms::Projection>(projection);
		}

		void setProjection(fmat && projection)
		{
			graphics()->updateUniform<Uniforms::Projection>(forward<fmat>(projection));
		}

		void setZoom(float zoom)
		{
			_zoom = zoom;
			updateViewport();
		}

		void setTickLength(milliseconds length)
		{
			_tickLength = length;
			_firstTick = _lastTick - (_tickLength * _ticks);
		}

		void update()
		{
			if(clock::now() - _lastTick > _tickLength)
			{
				_lastTick = clock::now();
				ticks_t ticks = (_lastTick - _firstTick) / _tickLength;

				for(; _ticks < ticks; ++_ticks)
				{
					for(auto & obj : _objects)
						obj->update(_ticks);
				}
			}
		}

	protected:
		Scene(const string & name) : Named(name)
		{
			setclass(Scene);
		}

		void draw(Graphics3D * graphics, const IntRect & viewport) const
		{
			for(auto & drawable : _drawables)
				drawable->draw(graphics, viewport, _zoom);
		}

		void updateViewport()
		{
			float invzoom = 1 / _zoom;
			float aspect = _widget->viewport().ratio() * invzoom;
			setProjection(fmat::ortho(-invzoom, invzoom, -aspect, aspect, -invzoom, invzoom));
		}

		Widget * _widget;
		Array<SceneObject> _objects;
		vector<Drawable *> _drawables;

		Handle<Camera> _camera;
		float _zoom = 0.01f;

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
