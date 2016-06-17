//---------------------------------------------------------------------------

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
			return fvec::default;
		}

		virtual const fquat & direction() const
		{
			return fquat::default;
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
		api(scene) Drawable(Scene * scene);
		virtual ~Drawable() {}

	protected:
		virtual void draw(Graphics3D & graphics, const IntRect & viewport, float zoom) const = 0;
	};

	class WorldObject : public SceneObject
	{
		deny_copy(WorldObject);

	public:
		WorldObject(Scene * scene, const fvec & pos = fvec::default) : SceneObject(scene), _pos(pos)
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
		OrientedObject(Scene * scene, const fvec & pos = fvec::default, const fquat & dir = fquat()) : WorldObject(scene, pos), _dir(dir)
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
		api(scene) Scene(Widget * widget, const string & name = "unknown scene");
		virtual api(scene) ~Scene();

		Graphics3D api(scene) & graphics() const;
		Widget     api(scene) & widget()   const;
		UISpace    api(scene) & space()    const;
		Camera     api(scene) * camera()   const;
		Viewport   api(scene)   viewport() const;

		void api(scene) setCamera(Camera * camera);

		void api(scene) invalidate() const;
		void api(scene) render() const;

		void api(scene) attach(Handle<SceneObject> obj);
		void api(scene) detach(SceneObject * obj);

		void api(scene) setTickLength(milliseconds length);
		void api(scene) update();

	protected:
		api(scene) Scene(const string & name);

		void api(scene) draw(Graphics3D & graphics, const IntRect & viewport) const;
		void api(scene) onWidgetResize(Handle<WidgetResizeMessage> & msg, Widget & w);

		Camera * _camera = nullptr;
		Widget * _widget = nullptr;
		Array<SceneObject> _objects;
		array_list<Drawable *> _drawables;

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
