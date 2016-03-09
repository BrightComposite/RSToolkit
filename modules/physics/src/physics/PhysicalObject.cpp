//---------------------------------------------------------------------------

#include <physics/PhysicalObject.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	void MotionState::getWorldTransform(btTransform & trans) const
	{
		trans.setRotation(object->_rot.q);
		trans.setOrigin(object->_pos.v);
	}

	void MotionState::setWorldTransform(const btTransform & trans)
	{
		object->_rot.set(trans.getRotation());
		object->_pos.set(trans.getOrigin());
	}

	void PhysicalObject::setMass(double mass)
	{
		btVector3 inertia;
		_shape->calculateLocalInertia(mass, inertia);
		_rigidBody->setMassProps(mass, inertia);
	}

	void PhysicalObject::setLinearVelocity(const DoubleVector & v)
	{
		_rigidBody->setLinearVelocity(v.v);
		_rigidBody->activate();
	}

	DoubleVector PhysicalObject::getLinearVelocity()
	{
		auto & vel = _rigidBody->getLinearVelocity();
		return {vel[0], vel[1], vel[2], vel[3]};
	}
}

//---------------------------------------------------------------------------
