//---------------------------------------------------------------------------

#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

//---------------------------------------------------------------------------

#include <physics/PhysicalLayer.h>

#include <core/Exception.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<>
	struct Cast<btVector3, vector>
	{
		static inline void cast(vector & out, const btVector3 & in)
		{
			out.set(in);
			out[3] = 1.0;
		}
	};

	template<>
	struct Cast<btQuaternion, quaternion>
	{
		static inline void cast(quaternion & out, const btQuaternion & in)
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

		// WHO THE HELL HAS MADE SUCH SIGNATURE?!!
		virtual btScalar addSingleResult(
			btManifoldPoint & cp, const btCollisionObjectWrapper * objA, int partA, int indexA, const btCollisionObjectWrapper * objB, int partB, int indexB
		) override
		{
			if(body == objA->m_collisionObject)
				points.push_back(cp.m_localPointA);
			else if(body == objB->m_collisionObject)
				points.push_back(cp.m_localPointB);
			else
				throw Exception("Body does not match either collision object");

			return 0;
		}
	};

	class Physical; 

	link_class(physics, Physical, Class<>);

	class MotionState : public btMotionState
	{
	public:
		MotionState(Spatial * object) : object(object) {}
		virtual ~MotionState() {}

		virtual api(physics) void getWorldTransform(btTransform & trans) const override;
		virtual api(physics) void setWorldTransform(const btTransform & trans) override;

		Spatial * object;
	};

	using Velocity = AlignedVector<scalar>;

	class Physical
	{
	public:
		Physical(Spatial * object)
		{
			_motionState = new MotionState(object);
		}

		Physical(Spatial * object, PhysicalLayer * world, btCollisionShape * shape, float mass = 0.0f) : Physical(object)
		{
			spawn(world, shape, mass);
		}

		virtual ~Physical()
		{
			_world->removeRigidBody(_rigidBody);
		}

		void spawn(PhysicalLayer * world, btCollisionShape * shape, float mass = 0.0)
		{
			if(_world != nullptr && _rigidBody != nullptr)
				_world->removeRigidBody(_rigidBody);

			_world = world;

			btVector3 inertia;
			shape->calculateLocalInertia(mass, inertia);

			btRigidBody::btRigidBodyConstructionInfo info(mass, _motionState, shape, inertia);
			info.m_restitution = 0.66f;
			info.m_friction = 0.8f;

			_rigidBody = new btRigidBody(info);
			_rigidBody->setUserPointer(this);
			_world->addRigidBody(_rigidBody);
		}

		virtual api(physics) void setMass(float mass);
		virtual api(physics) void setLinearVelocity(const Velocity & v);
		api(physics) Velocity getLinearVelocity();

		virtual void contactWith(Physical * physical, const ContactInfo & info) {}

	protected:
		PhysicalLayer * _world;

		Unique<btRigidBody>		 _rigidBody;
		Unique<btMotionState>	 _motionState;
	};
}

//---------------------------------------------------------------------------
#endif
