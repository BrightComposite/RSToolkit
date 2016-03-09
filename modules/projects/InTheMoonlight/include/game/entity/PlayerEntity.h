//---------------------------------------------------------------------------

#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

//---------------------------------------------------------------------------

#include <scene/Scene.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class PlayerEntity : public Object
	{
	public:
		PlayerEntity(Handle<SceneObject> object) : _object(object)
		{
		}

		virtual ~PlayerEntity()
		{
			s->detach(_object);
		}

		SceneObject * object()
		{
			return _object;
		}

		const SceneObject * object() const
		{
			return _object;
		}

		Scene * scene() const
		{
			return _object->scene();
		}

	protected:
		
		Handle<SceneObject> _object;
	};
}

//---------------------------------------------------------------------------
#endif
