//---------------------------------------------------------------------------

#ifndef PHYSICAL_WORLD_H
#define PHYSICAL_WORLD_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <core/container/Array.h>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class PhysicalObject;

	struct ContactInfo
	{
		btVector3 normal;
		btVector3 pos;
		btScalar force;
	};

	class PhysicalWorld : public Object, public btDiscreteDynamicsWorld
	{
	public:
		PhysicalWorld(btDispatcher * dispatcher, btBroadphaseInterface * broadphase, btConstraintSolver * solver, btCollisionConfiguration * config) : btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config)
		{
			setInternalTickCallback(PhysicalWorld::update, this);
			setGravity({0.0, -10.0, 0.0});
		}

	protected: public_for_handle(PhysicalWorld);
		using btDiscreteDynamicsWorld::operator new;
		using btDiscreteDynamicsWorld::operator delete;
		using btDiscreteDynamicsWorld::operator new[];
		using btDiscreteDynamicsWorld::operator delete[];

		static void update(btDynamicsWorld * world, btScalar timeStep);
	};
}

//---------------------------------------------------------------------------
#endif
