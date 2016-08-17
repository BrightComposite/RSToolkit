//---------------------------------------------------------------------------

#pragma once

#ifndef PHYSICAL_WORLD_H
#define PHYSICAL_WORLD_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <container/ArrayList.h>

#include <math/Vector.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Physical;

	struct ContactInfo
	{
		fvec normal;
		fvec pos;
		float force;
	};

	class PhysicalWorld : public Object, public btDiscreteDynamicsWorld
	{
		deny_copy(PhysicalWorld);

	public:
		PhysicalWorld(btDispatcher * dispatcher, btBroadphaseInterface * broadphase, btConstraintSolver * solver, btCollisionConfiguration * config) : btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config)
		{
			setInternalTickCallback(PhysicalWorld::update, this);
			setGravity({0.0f, -10.0f, 0.0f});
		}

	protected:
		friend_handle;
		using btDiscreteDynamicsWorld::operator new;
		using btDiscreteDynamicsWorld::operator delete;
		using btDiscreteDynamicsWorld::operator new[];
		using btDiscreteDynamicsWorld::operator delete[];

		static api(physics) void update(btDynamicsWorld * world, btScalar timeStep);
	};
}

//---------------------------------------------------------------------------
#endif
