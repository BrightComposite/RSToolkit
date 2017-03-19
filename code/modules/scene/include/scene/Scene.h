//---------------------------------------------------------------------------

#pragma once

#ifndef SCENE_H
#define SCENE_H

//---------------------------------------------------------------------------

#include <ui/UISpace.h>
#include <graphics/Graphics3D.h>
#include <space/Spatial.h>

#include <chrono>

//---------------------------------------------------------------------------

namespace asd
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
		friend class SceneLayer;

		deny_copy(Scene);

	public:
		virtual api(scene) ~Scene();

		virtual Handle<Scene> clone(Widget * w) const
		{
			return Handle<Scene, Scene>(w);
		}

		api(scene) Graphics3D & graphics() const;
		api(scene) Widget     & widget()   const;
		api(scene) UISpace    & space()    const;
		api(scene) Camera     * camera()   const;
		api(scene) Viewport     viewport() const;

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

		api(scene) matrix normalMatrix(const matrix & model) const;

		static Handle<Scene> create(Widget * widget)
		{
			Handle<Scene, Scene> h(widget);
			Scene::construct(h);

			return h;
		}

	protected:
		friend_owned_handle(Scene, Scene);
		static api(scene) Handle<Scene> construct(const Handle<Scene> & scene);

		api(scene) Scene(Widget * widget);
		Scene() { setclass(Scene); }

		virtual api(scene) void draw(Graphics3D & graphics, const IntRect & viewport) const;
		virtual api(scene) void onWidgetResize(Handle<WidgetResizeMessage> & msg, Widget & w);

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

	class SceneLayer : public WidgetLayer
	{
	public:
		SceneLayer(const Handle<Scene> & scene) : WidgetLayer(limits<int>::max()), _scene(scene) {}

		virtual Handle<WidgetLayer> clone(Widget * widget) const
		{
			return Handle<SceneLayer>(_scene->clone(widget));
		}

		const Handle<Scene> & scene() const
		{
			return _scene;
		}

	protected:
		virtual void draw(Widget * w) override
		{
			_scene->draw(_scene->graphics(), scaleToSquare(w->absRegion()));
		}

		Handle<Scene> _scene;
	};

	create_layer_component(scene, SceneLayer);

	class SceneProvider
	{
		Dictionary<string, Scene> _scenes;
	};
}

//---------------------------------------------------------------------------
#endif
