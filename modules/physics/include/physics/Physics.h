//---------------------------------------------------------------------------

#ifndef PHYSICS_H
#define PHYSICS_H

//---------------------------------------------------------------------------

#include <physics/PhysicalObject.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	struct ContactSensorCallback : public btCollisionWorld::ContactResultCallback
	{
		using Base = btCollisionWorld::ContactResultCallback;

		ContactSensorCallback(btRigidBody * body, array_list<btVector3> & points)
			: Base(), body(body), points(points) {}

		btRigidBody * body;
		array_list<btVector3> & points;

		virtual bool needsCollision(btBroadphaseProxy * proxy) const
		{
			if(!Base::needsCollision(proxy))
				return false;

			return body->checkCollideWithOverride(static_cast<btCollisionObject *>(proxy->m_clientObject));
		}

		virtual btScalar addSingleResult
		(
			btManifoldPoint & cp,
			const btCollisionObjectWrapper * colObj0, int partId0, int index0,
			const btCollisionObjectWrapper * colObj1, int partId1, int index1
		)
		{
			if(colObj0->m_collisionObject == body)
				points.push_back(cp.m_localPointA);
			else
				if(colObj1->m_collisionObject == body)
					points.push_back(cp.m_localPointB);
			else
				throw Exception("body does not match either collision object");

			return 0;
		}
	};

	template<>
	struct Cast<btVector3, dvec>
	{
		static inline void cast(dvec & out, const btVector3 & in)
		{
			out.set(in);
			out[3] = 1.0;
		}
	};

	template<>
	struct Cast<btQuaternion, DoubleQuaternion>
	{
		static inline void cast(DoubleQuaternion & out, const btQuaternion & in)
		{
			out.set(in);
		}
	};
}

//---------------------------------------------------------------------------
#endif
