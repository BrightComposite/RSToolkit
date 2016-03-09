//---------------------------------------------------------------------------

#ifndef SCENE_H
#define SCENE_H

//---------------------------------------------------------------------------

#include <core/container/Set.h>
#include <math/Vector.h>
#include <graphics/Graphics3D.h>
#include <ui/WindowAdapter.h>

#include <chrono>

//---------------------------------------------------------------------------

namespace Rapture
{
	using namespace std::chrono;
	using namespace std::chrono_literals;

	using ticks_t = long long;

	link_class(SceneObject, MetaClass<Object>);
	link_class(Drawable, MetaBase);
	link_class(DrawableObject, MetaClass<SceneObject, Drawable>);
	link_class(Scene, MetaClass<Object>);

	class SceneObject : public Object
	{
		friend class Scene;

	public:
		SceneObject(Scene * scene);
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

	class Scene : public Object
	{
		friend class Drawable;
		friend class SceneObject;

	protected:
		class SceneLayer : public Layer
		{
		public:
			SceneLayer(Scene * scene) : _scene(scene) {}

			virtual void draw(const Widget * widget, const IntRect & clipRegion) const
			{
				_scene->draw(static_cast<Graphics3D *>(widget->graphics()), scaleToSquare(widget->absRegion()));
			}

		protected:
			Scene * _scene;
		};

		typedef std::chrono::high_resolution_clock clock;
		typedef time_point<clock> time_marker;

	public:
		Scene(Handle<Widget> widget) : Scene()
		{
			if(widget->graphics() notkindof (Graphics3D))
				throw Exception("Widget for the scene must support 3D graphics!");

			_widget = widget;
			widget->attach(handle<SceneLayer>(this), INT_MAX);

			_firstTick = _lastTick = clock::now();
			_ticks = 0;
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

		void invalidate() const
		{
			adapter()->invalidate(_widget);
		}

		template<class T, typename ... A,
			require_i(1,
				based_on(T, SceneObject) &&
				can_construct(T, Scene *, A...)
				)>
		Handle<T> append(A && ... args)
		{
			return handle<T>(this, forward<A>(args)...);
		}

		void attach(Handle<SceneObject> obj);
		void detach(SceneObject * obj);

		Widget * widget() const
		{
			return _widget;
		}

		WindowAdapter * adapter() const
		{
			return _widget->adapter();
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
		Scene() { setclass(Scene); }

		void draw(Graphics3D * graphics, const IntRect & viewport) const
		{
			for(auto & drawable : _drawables)
				drawable->draw(graphics, viewport, _zoom);
		}

		Widget * _widget;
		Array<SceneObject> _objects;
		PointerArray<Drawable> _drawables;

		milliseconds _tickLength = 1ms;
		time_marker _lastTick;
		time_marker _firstTick;

		ticks_t _ticks = 0;
		float _zoom = 0.02f;
	};
}

//---------------------------------------------------------------------------
#endif