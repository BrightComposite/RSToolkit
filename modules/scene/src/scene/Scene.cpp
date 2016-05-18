//---------------------------------------------------------------------------

#include <scene/Scene.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	SceneObject::SceneObject(Scene * scene, const DoubleVector & pos) : _scene(scene), _pos(pos)
	{
		setclass(SceneObject);
		scene->_objects.push_back(this);
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
}