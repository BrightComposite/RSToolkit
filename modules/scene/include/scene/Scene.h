//---------------------------------------------------------------------------

#pragma once

#ifndef SCENE_H
#define SCENE_H

//---------------------------------------------------------------------------

#include <ui/UISpace.h>
#include <graphics/Graphics3D.h>

#include <chrono>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Scene;
	class SceneObject;
	class Drawable;
	class WorldObject;
	class OrientedObject;

	class Camera;

	using namespace std::chrono;
	using namespace std::chrono_literals;

	using ticks_t = long long;

	link_class(scene, Scene, Class<Object>);
	link_class(scene, SceneObject, Class<Object>);
	link_class(scene, Drawable, Class<>);
	link_class(scene, WorldObject, Class<SceneObject>);
	link_class(scene, OrientedObject, Class<WorldObject>);

	class SceneObject : public Object
	{
		friend class Scene;

		deny_copy(SceneObject);

	public:
		api(scene) SceneObject(Scene * scene);
		virtual ~SceneObject() {}

		Scene * scene() const
		{
			return _scene;
		}

		virtual const fvec & position() const
		{
			return fvec::identity;
		}

		virtual const fquat & direction() const
		{
			return fquat::identity;
		}

		virtual void setPosition(const fvec & pos) {}
		virtual void setDirection(const fquat & dir) {}
		virtual void move(const fvec & offset) {}
		virtual void rotate(const fquat & rot) {}

		void move(float x, float y, float z)
		{
			move({x, y, z});
		}

		void moveX(float x)
		{
			move({x, 0, 0});
		}

		void moveY(float y)
		{
			move({0, y, 0});
		}

		void moveZ(float z)
		{
			move({0, 0, z});
		}

		void rotate(const fvec & axis, float angle)
		{
			rotate({axis, angle});
		}

		void rotateX(float angle)
		{
			rotate(fvec::positiveX, angle);
		}

		void rotateY(float angle)
		{
			rotate(fvec::positiveY, angle);
		}

		void rotateZ(float angle)
		{
			rotate(fvec::positiveZ, angle);
		}

	protected:
		virtual void update(ticks_t ticks) {}

		Scene * _scene;
	};

	class Drawable
	{
		friend class Scene;

		deny_copy(Drawable);

	public:
		api(scene) Drawable(Scene * scene, bool transparent = false);
		virtual ~Drawable() {}

		bool transparent() const
		{
			return _transparent;
		}

	protected:
		virtual void draw(Graphics3D & graphics, const IntRect & viewport, float zoom) const = 0;

		bool _transparent = false;
	};

	class WorldObject : public SceneObject
	{
		deny_copy(WorldObject);

	public:
		WorldObject(Scene * scene, const fvec & pos = fvec::identity) : SceneObject(scene), _pos(pos)
		{
			setclass(WorldObject);
		}

		virtual ~WorldObject() {}

		virtual const fvec & position() const override
		{
			return _pos;
		}

		virtual void setPosition(const fvec & pos) override
		{
			_pos = pos;
		}

		virtual void move(const fvec & offset) override
		{
			_pos += offset;
		}

	protected:
		fvec _pos;
	};

	class OrientedObject : public WorldObject
	{
		deny_copy(OrientedObject);

	public:
		OrientedObject(Scene * scene, const fvec & pos = fvec::identity, const fquat & dir = fquat()) : WorldObject(scene, pos), _dir(dir)
		{
			setclass(OrientedObject);
		}

		virtual ~OrientedObject() {}

		virtual const fquat & direction() const override
		{
			return _dir;
		}

		virtual void setDirection(const fquat & dir) override
		{
			_dir = dir;
		}

		virtual void move(const fvec & offset) override
		{
			_pos += _dir.applyTo(offset);
		}

		virtual void rotate(const fquat & rot) override
		{
			_dir.rotateBy(rot);
		}

	protected:
		fquat _dir;
	};

	class Scene : public Object, public Named, public Connector
	{
		friend class Drawable;
		friend class SceneObject;
		friend class Widget;

		deny_copy(Scene);

	public:
		class Component : public WidgetLayerComponent 
		{
			class Layer : public WidgetLayer
			{
			public:
				Layer(Scene * scene) : WidgetLayer(INT_MAX), _scene(scene) {}

			protected:
				virtual void draw(Widget * w) override
				{
					_scene->draw(_scene->graphics(), scaleToSquare(w->absRegion()));
				}

				Scene * _scene;
			};

		public:
			Component(Widget * widget, Scene * scene) : WidgetLayerComponent(widget, UniqueHandle<Layer>(scene)) {}
			virtual ~Component() {}
		};

		typedef std::chrono::high_resolution_clock clock;
		typedef time_point<clock> time_marker;

		api(scene) Scene(Widget * widget, const string & name = "unknown scene");
		virtual ~Scene() {}

		Graphics3D api(scene) & graphics() const;
		Widget     api(scene) & widget()   const;
		UISpace    api(scene) & space()    const;
		Camera     api(scene) * camera()   const;
		Viewport   api(scene)   viewport() const;

		api(scene) void setCamera(Camera * camera);

		api(scene) void invalidate() const;
		api(scene) void render() const;

		template<class Obj, class ... A, useif <based_on<Obj, SceneObject>::value, can_construct<Obj, Scene *, A...>::value> endif>
		Handle<Obj> append(A &&... args)
		{
			return Handle<Obj>(this, forward<A>(args)...);
		}

		api(scene) void attach(Handle<SceneObject> obj);
		api(scene) void detach(SceneObject * obj);

		api(scene) void setTickLength(milliseconds length);
		api(scene) void update();

	protected:
		Scene(const string & name) : Named(name) { setclass(Scene); }

		api(scene) void draw(Graphics3D & graphics, const IntRect & viewport) const;
		api(scene) void onWidgetResize(Handle<WidgetResizeMessage> & msg, Widget & w);

		Camera * _camera = nullptr;
		Widget * _widget = nullptr;
		ArrayList<SceneObject> _objects;
		array_list<Drawable *> _opaque;
		array_list<Drawable *> _transparent;

		milliseconds _tickLength = 1ms;
		time_marker _lastTick;
		time_marker _firstTick;

		ticks_t _ticks = 0;
	};

	create_component(scene, Scene::Component);

	class SceneProvider
	{
		Map<string, Scene> _scenes;
	};
}

//---------------------------------------------------------------------------
#endif
