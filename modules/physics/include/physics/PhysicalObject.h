//---------------------------------------------------------------------------

#ifndef PHYSICAL_OBJECT_H
#define PHYSICAL_OBJECT_H

//---------------------------------------------------------------------------

#include <scene/Scene.h>
#include <physics/PhysicalWorld.h>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class PhysicalObject;

	link_class(PhysicalObject, MetaClass<SceneObject>);

	class MotionState : public btMotionState
	{
	public:
		MotionState(PhysicalObject * object) : object(object) {}
		virtual ~MotionState() {}

		virtual void getWorldTransform(btTransform & worldTrans) const;
		virtual void setWorldTransform(const btTransform & worldTrans);

		PhysicalObject * object;
	};

	class PhysicalObject : public SceneObject
	{
		friend class MotionState;

	public:
		PhysicalObject(Scene * scene, PhysicalWorld * world, btCollisionShape * shape, const DoubleVector & pos = DoubleVector({0.0, 0.0, 0.0, 1.0}), double mass = 0.0) : SceneObject(scene), _shape(shape), _world(world), _rot(0.0, 0.0, 0.0)
		{
			setclass(PhysicalObject);

			_motionState = new MotionState(this);
			_pos = pos;

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
		virtual void update(long long ticks)
		{
		}

		DoubleQuaternion _rot;
		PhysicalWorld * _world;
		UniqueHandle<btCollisionShape> _shape;
		UniqueHandle<btRigidBody> _rigidBody;
		UniqueHandle<btMotionState> _motionState;
	};
}

//---------------------------------------------------------------------------
#endif
