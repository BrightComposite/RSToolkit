//---------------------------------------------------------------------------

#include <scene/Scene.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(Scene);
	implement_link(SceneObject);
	implement_link(Drawable);
	implement_link(DrawableObject);

	SceneObject::SceneObject(Scene * scene, const DoubleVector & pos) : _scene(scene), _pos(pos)
	{
		setclass(SceneObject);
		scene->_objects.emplace_back(this);
	}

	Drawable::Drawable(Scene * scene)
	{
		scene->_drawables.push_back(this);
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
			_drawables.push_back(dynamic_cast<Drawable *>(static_cast<SceneObject *>(obj)));
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

			for(auto i = _drawables.begin(); i != _drawables.end(); ++i)
			{
				if(*i == drawable)
				{
					_drawables.erase(i);
					break;
				}
			}
		}
	}

	void Scene::setOrtho()
	{
		_projectionMode = ProjectionMode::Ortho;

		float aspect = _widget->viewport().ratio();
		setProjection(fmat::ortho(-1.0f, 1.0f, -aspect, aspect, 0.0f, 10.0f / _zoom));
	}

	void Scene::setPerspective()
	{
		_projectionMode = ProjectionMode::Perspective;
		setProjection(fmat::perspective(_fov, _widget->viewport().ratio(), 0.01f, 10 / _zoom));
	}

	void Scene::onWidgetResize(Handle<WidgetResizeMessage> & msg, Widget & w)
	{
		if(_projectionMode == ProjectionMode::Ortho)
			setOrtho();
		else
			setPerspective();
	}

	void Scene::setProjection(fmat && projection)
	{
		graphics().updateUniform<Uniforms::Projection>(forward<fmat>(projection));
	}

	void Scene::setZoom(float zoom)
	{
		_zoom = zoom;

		if(_projectionMode == ProjectionMode::Ortho)
			setOrtho();
		else
			setPerspective();
	}

	void Scene::setFieldOfView(float fov)
	{
		_fov = fov;

		if(_projectionMode == ProjectionMode::Perspective)
			setPerspective();
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

	void Camera::update()
	{
		_scene->graphics().updateUniform<Uniforms::View>(fmat::lookTo(position, direction.forward(), fvec::up));
	}

	void Camera::move(fvec offset)
	{
		position += direction.applyTo(offset);
	}

	void Camera::move(float x, float y, float z)
	{
		position += direction.applyTo({x, y, z});
	}

	void Camera::moveX(float x)
	{
		position += direction.applyTo({x, 0, 0});
	}

	void Camera::moveY(float y)
	{
		position += direction.applyTo({0, y, 0});
	}

	void Camera::moveZ(float z)
	{
		position += direction.applyTo({0, 0, z});
	}
}