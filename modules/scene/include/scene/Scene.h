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
	class SceneObject;
	class Drawable;
	class Scene;

	class Camera;

	using namespace std::chrono;
	using namespace std::chrono_literals;

	using ticks_t = long long;
	typedef std::chrono::high_resolution_clock clock;
	typedef time_point<clock> time_marker;

	link_class(scene, SceneObject, Class<Object>);
	link_class(scene, Drawable, Class<>);
	link_class(scene, Scene, Class<Object>);

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

	protected:
		virtual void update(ticks_t ticks) {}

		Scene * _scene;
	};

	class SceneObjectComponent : public Shared
	{
		component_base(SceneObjectComponent);

	public:
		SceneObjectComponent(SceneObject * object) : _object(object) {}

		SceneObject * object()
		{
			return _object;
		}

		const SceneObject * object() const
		{
			return _object;
		}

	protected:
		SceneObject * _object;
	};

	create_component_pool(scene, SceneObjectComponent);

	using SceneObjectComponents = ComponentSet<SceneObjectComponent>;

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

		virtual void draw(Graphics3D & graphics, const IntRect & viewport, float zoom) const = 0;

	protected:
		bool _transparent = false;
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

		api(scene) Scene(Widget * widget, const string & name = "unknown scene");
		virtual ~Scene() {}

		Graphics3D api(scene) & graphics() const;
		Widget     api(scene) & widget()   const;
		UISpace    api(scene) & space()    const;
		Camera     api(scene) * camera()   const;
		Viewport   api(scene)   viewport() const;

		api(scene) void setCamera(Camera * camera);

		api(scene) void render() const;

		template<class Obj, class ... A, useif<based_on<Obj, SceneObject>::value, can_construct<Obj, Scene *, A...>::value>>
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

		virtual api(scene) void draw(Graphics3D & graphics, const IntRect & viewport) const;
		api(scene) void onWidgetResize(Handle<WidgetResizeMessage> & msg, Widget & w);

		Camera * _camera = nullptr;
		Widget * _widget = nullptr;
		ArrayList<SceneObject> _objects;
		array_list<Drawable *> _opaque;
		array_list<Drawable *> _transparent;

		milliseconds _tickLength = 5ms;
		time_marker _lastTick;
		time_marker _firstTick;

		ticks_t _ticks = 0;
	};

	create_component(scene, Scene::Component);

	class SceneProvider
	{
		Dictionary<string, Scene> _scenes;
	};
}

//---------------------------------------------------------------------------
#endif
