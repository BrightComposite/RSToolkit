//---------------------------------------------------------------------------

#include <physics/PhysicalObject.h>
#include <physics/Physics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	void MotionState::getWorldTransform(btTransform & trans) const
	{
		trans.setRotation(object->_rot.elements);
		trans.setOrigin(object->_pos.elements);
	}

	void MotionState::setWorldTransform(const btTransform & trans)
	{
		object->_rot = trans.getRotation();
		object->_pos = trans.getOrigin();
	}

	void PhysicalObject::setMass(double mass)
	{
		btVector3 inertia;
		_shape->calculateLocalInertia(mass, inertia);
		_rigidBody->setMassProps(mass, inertia);
	}

	void PhysicalObject::setLinearVelocity(const DoubleVector & v)
	{
		_rigidBody->setLinearVelocity(v.elements);
		_rigidBody->activate();
	}

	DoubleVector PhysicalObject::getLinearVelocity()
	{
		return _rigidBody->getLinearVelocity();
	}
}

//---------------------------------------------------------------------------
