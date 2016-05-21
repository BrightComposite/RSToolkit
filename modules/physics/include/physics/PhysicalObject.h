//---------------------------------------------------------------------------

#ifndef PHYSICAL_OBJECT_H
#define PHYSICAL_OBJECT_H

//---------------------------------------------------------------------------

#include <scene/Scene.h>
#include <physics/PhysicalWorld.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	declare_and_link(PhysicalObject, Class<SceneObject>);

	class MotionState : public btMotionState
	{
	public:
		MotionState(PhysicalObject * object) : object(object) {}
		virtual ~MotionState() {}

		virtual void getWorldTransform(btTransform & trans) const override;
		virtual void setWorldTransform(const btTransform & trans) override;

		PhysicalObject * object;
	};

	class PhysicalObject : public SceneObject
	{
		friend class MotionState;

	public:
		PhysicalObject(Scene * scene, PhysicalWorld * world, btCollisionShape * shape, const DoubleVector & pos = DoubleVector::positiveW, double mass = 0.0) : SceneObject(scene, pos), _shape(shape), _world(world)
		{
			setclass(PhysicalObject);
			_motionState = new MotionState(this);

			btVector3 inertia;
			_shape->calculateLocalInertia(mass, inertia);

			btRigidBody::btRigidBodyConstructionInfo info(mass, _motionState, _shape, inertia);
			info.m_restitution = 0.66;
			info.m_friction = 0.8;

			_rigidBody = new btRigidBody(info);
			_rigidBody->setUserPointer(this);
			_world->addRigidBody(_rigidBody);
		}

		virtual ~PhysicalObject()
		{
			_world->removeRigidBody(_rigidBody);
		}

		virtual void setMass(double mass);
		virtual void setLinearVelocity(const DoubleVector & v);
		DoubleVector getLinearVelocity();

		virtual void contactWith(PhysicalObject * obj, const ContactInfo & info) {}

	protected:
		virtual void update(long long ticks) {}

		DoubleQuaternion _rot;
		PhysicalWorld * _world;
		UniqueHandle<btCollisionShape> _shape;
		UniqueHandle<btRigidBody> _rigidBody;
		UniqueHandle<btMotionState> _motionState;
	};
}

//---------------------------------------------------------------------------
#endif
