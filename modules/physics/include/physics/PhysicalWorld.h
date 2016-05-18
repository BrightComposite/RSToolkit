//---------------------------------------------------------------------------

#ifndef PHYSICAL_WORLD_H
#define PHYSICAL_WORLD_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <core/container/Array.h>

#include <math/Vector.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class PhysicalObject;

	struct ContactInfo
	{
		dvec normal;
		dvec pos;
		double force;
	};

	class PhysicalWorld : public Object, public btDiscreteDynamicsWorld
	{
	public:
		PhysicalWorld(btDispatcher * dispatcher, btBroadphaseInterface * broadphase, btConstraintSolver * solver, btCollisionConfiguration * config) : btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config)
		{
			setInternalTickCallback(PhysicalWorld::update, this);
			setGravity({0.0, -10.0, 0.0});
		}

	protected:
		friend_handle;
		using btDiscreteDynamicsWorld::operator new;
		using btDiscreteDynamicsWorld::operator delete;
		using btDiscreteDynamicsWorld::operator new[];
		using btDiscreteDynamicsWorld::operator delete[];

		static void update(btDynamicsWorld * world, btScalar timeStep);
	};
}

//---------------------------------------------------------------------------
#endif
