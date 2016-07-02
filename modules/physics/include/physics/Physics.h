//---------------------------------------------------------------------------

#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

//---------------------------------------------------------------------------

#include <scene/Scene.h>
#include <physics/PhysicalWorld.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<>
	struct Cast<btVector3, fvec>
	{
		static inline void cast(fvec & out, const btVector3 & in)
		{
			out.set(in);
			out[3] = 1.0;
		}
	};

	template<>
	struct Cast<btQuaternion, fquat>
	{
		static inline void cast(fquat & out, const btQuaternion & in)
		{
			out.set(in);
		}
	};

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

	class Physical; 

	link_class(physics, Physical, Class<>);

	class MotionState : public btMotionState
	{
	public:
		MotionState(Physical * physical) : physical(physical) {}
		virtual ~MotionState() {}

		virtual api(physics) void getWorldTransform(btTransform & trans) const override;
		virtual api(physics) void setWorldTransform(const btTransform & trans) override;

		Physical * physical;
	};

	class Physical : public Object
	{
		friend class MotionState;

	public:
		Physical(SceneObject * object, PhysicalWorld * world, btCollisionShape * shape, float mass = 0.0) : _object(object), _shape(shape), _world(world)
		{
			setclass(Physical);
			_motionState = new MotionState(this);

			btVector3 inertia;
			_shape->calculateLocalInertia(mass, inertia);

			btRigidBody::btRigidBodyConstructionInfo info(mass, _motionState, _shape, inertia);
			info.m_restitution = 0.66f;
			info.m_friction = 0.8f;

			_rigidBody = new btRigidBody(info);
			_rigidBody->setUserPointer(this);
			_world->addRigidBody(_rigidBody);
		}

		virtual ~Physical()
		{
			_world->removeRigidBody(_rigidBody);
		}

		virtual api(physics) void setMass(float mass);
		virtual api(physics) void setLinearVelocity(const fvec & v);
		api(physics) fvec getLinearVelocity();

		virtual void contactWith(Physical * physical, const ContactInfo & info) {}

	protected:
		SceneObject * _object;
		PhysicalWorld * _world;
		UniqueHandle<btCollisionShape> _shape;
		UniqueHandle<btRigidBody> _rigidBody;
		UniqueHandle<btMotionState> _motionState;
	};
}

//---------------------------------------------------------------------------
#endif
