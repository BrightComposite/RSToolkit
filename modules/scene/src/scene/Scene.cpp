//---------------------------------------------------------------------------

#include <scene/Scene.h>
#include <scene/Camera.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(Scene);
	implement_link(SceneObject);
	implement_link(Drawable);
	implement_link(WorldObject);
	implement_link(OrientedObject);

	SceneObject::SceneObject(Scene * scene) : _scene(scene)
	{
		setclass(SceneObject);
		scene->_objects.emplace_back(this);
	}

	Drawable::Drawable(Scene * scene, bool transparent) : _transparent(transparent)
	{
		auto & list = _transparent ? scene->_transparent : scene->_opaque;
		list.push_back(this);
	}

	Scene::Scene(Widget * widget, const string & name) : Scene(name)
	{
		if(widget->graphics() notkindof (Graphics3D))
			throw Exception("Widget for the scene must support 3D graphics!");

		_widget = widget;
		_widget->append<Component>(this);
		connect(this, &Scene::onWidgetResize, *_widget);

		_firstTick = _lastTick = clock::now();
		_ticks = 0;
	}

	Graphics3D & Scene::graphics() const
	{
		return *static_cast<Graphics3D *>(_widget->graphics());
	}

	Widget & Scene::widget() const
	{
		return *_widget;
	}

	UISpace & Scene::space() const
	{
		return *_widget->space();
	}

	Viewport Scene::viewport() const
	{
		return _widget->viewport();
	}

	Camera * Scene::camera() const
	{
		return _camera;
	}

	void Scene::invalidate() const
	{
		space().invalidate(_widget);
	}

	void Scene::render() const
	{
		space().invalidate(_widget);
		space().validate();
	}

	void Scene::setCamera(Camera * camera)
	{
		_camera = camera;
	}

	void Scene::attach(Handle<SceneObject> obj)
	{
		auto & scene = obj->_scene;

		if(scene != nullptr)
		{
			scene->detach(obj);
			scene = this;
		}

		_objects.push_back(obj);

		if(obj kindof(Drawable))
		{
			auto * drawable = dynamic_cast<Drawable *>(static_cast<SceneObject *>(obj));
			auto & list = drawable->transparent() ? _transparent : _opaque;
			list.push_back(drawable);
		}
	}

	void Scene::detach(SceneObject * obj)
	{
		if(obj->_scene != this)
			return;

		for(auto i = _objects.begin(); i != _objects.end(); ++i)
		{
			if(*i == obj)
			{
				_objects.erase(i);
				break;
			}
		}

		if(obj kindof(Drawable))
		{
			auto * drawable = dynamic_cast<Drawable *>(obj);
			auto & list = drawable->transparent() ? _transparent : _opaque;

			for(auto i = list.begin(); i != list.end(); ++i)
			{
				if(*i == drawable)
				{
					list.erase(i);
					break;
				}
			}
		}
	}

	void Scene::onWidgetResize(Handle<WidgetResizeMessage> & msg, Widget & w)
	{
		if(_camera)
			_camera->updateProjection();
	}

	void Scene::draw(Graphics3D & g, const IntRect & viewport) const
	{
		for(auto & drawable : _opaque)
		{
			drawable->draw(g, viewport, _camera ? _camera->zoom() : 0.0f);
		}

		//auto dt = hold(g.depthTestState(), false);

		for(auto & drawable : _transparent)
			drawable->draw(g, viewport, _camera ? _camera->zoom() : 0.0f);
	}

	void Scene::setTickLength(milliseconds length)
	{
		_tickLength = length;
		_firstTick = _lastTick - (_tickLength * _ticks);
	}

	void Scene::update()
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

	void Camera::setProjectionMode(ProjectionMode mode)
	{
		if(_projectionMode == mode)
			return;

		_projectionMode = mode;
		updateProjection();
	}

	void Camera::updateProjection()
	{
		float aspect = _scene->viewport().ratio();
		float z = 1.0f / _zoom;

		switch(_projectionMode)
		{
			case ProjectionMode::Ortho:
			{
				_scene->graphics().updateUniform<Uniforms::Projection>(floatm::orthot(-1.0f / aspect, 1.0f / aspect, -1.0f, 1.0f, -z, z));
				break;
			}

			case ProjectionMode::Perspective:
			{
				_scene->graphics().updateUniform<Uniforms::Projection>(floatm::perspectivet(_fov, aspect, 0.01f, 2 * z));
				break;
			}
		}
	}

	void Camera::setZoom(float zoom)
	{
		_zoom = zoom;
	}

	void Camera::setFieldOfView(float fov)
	{
		_fov = fov;
	}

	void Camera::setRotation(const floatq & rot)
	{
		_rot = rot;
	}

	void Camera::rotate(const floatq & rot)
	{
		_rot->rotateBy(rot);
	}

	void Camera::setPitch(float value)
	{
		_angles->x = fmath::clamp(value, -fmath::half_pi, fmath::half_pi);
		_rot->fromEuler(_angles);
	}

	void Camera::setYaw(float value)
	{
		_angles->y = fmath::rmod(value);
		_rot->fromEuler(_angles);
	}

	void Camera::setRoll(float value)
	{
		_angles->z = value;
		_rot->fromEuler(_angles);
	}

	void Camera::setAngles(float pitch, float yaw, float roll)
	{
		_angles->x = fmath::clamp(pitch, -fmath::pi * 0.49f, fmath::pi * 0.49f);
		_angles->y = fmath::rmod(yaw);
		_angles->z = roll;

		_rot = floatq(floatv::up, _angles->y) * floatq(floatv::right, _angles->x);
	}

	void Camera::addPitch(float value)
	{
		setPitch(_angles->x + value);
	}

	void Camera::addYaw(float value)
	{
		setYaw(_angles->y + value);
	}

	void Camera::addRoll(float value)
	{
		setRoll(_angles->z + value);
	}

	void Camera::addAngles(float pitch, float yaw, float roll)
	{
		setAngles(_angles->x + pitch, _angles->y + yaw, _angles->z + roll);
	}

	void Camera::update()
	{
		_scene->graphics().updateUniform<Uniforms::View>(floatm::lookTo(_pos, _rot->forward(), _rot->up()).transpose());
	}
}