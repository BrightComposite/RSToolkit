//---------------------------------------------------------------------------

#include <physics/PlaneObject.h>
#include <physics/PhysicalWorld.h>
#include <physics/Physics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(Physical);
	implement_link(PlaneObject);

	void MotionState::getWorldTransform(btTransform & trans) const
	{
		trans.setRotation(physical->_object->direction().intrinsic);
		trans.setOrigin(physical->_object->position().intrinsic);
	}

	void MotionState::setWorldTransform(const btTransform & trans)
	{
		physical->_object->setDirection(trans.getRotation());
		physical->_object->setPosition(trans.getOrigin());
	}

	void Physical::setMass(float mass)
	{
		btVector3 inertia;
		_shape->calculateLocalInertia(mass, inertia);
		_rigidBody->setMassProps(mass, inertia);
	}

	void Physical::setLinearVelocity(const fvec & v)
	{
		_rigidBody->setLinearVelocity(v.intrinsic);
		_rigidBody->activate();
	}

	fvec Physical::getLinearVelocity()
	{
		return _rigidBody->getLinearVelocity();
	}
	
	void PhysicalWorld::update(btDynamicsWorld * world, btScalar timeStep)
	{
		auto * w = static_cast<PhysicalWorld *>(world->getWorldUserInfo());

		int num = world->getDispatcher()->getNumManifolds();
		ContactInfo contactInfo;

		for(int i = 0; i < num; i++)
		{
			auto * manifold = world->getDispatcher()->getManifoldByIndexInternal(i);

			if(manifold->getNumContacts() == 0)
				continue;

			auto * obj0 = static_cast<Physical *>(manifold->getBody0() ? manifold->getBody0()->getUserPointer() : nullptr);
			auto * obj1 = static_cast<Physical *>(manifold->getBody1() ? manifold->getBody1()->getUserPointer() : nullptr);

			for(int j = 0; j < manifold->getNumContacts(); j++)
			{
				auto & pt = manifold->getContactPoint(j);

				if(pt.getDistance() < 0.0)
				{
					contactInfo.force = pt.m_appliedImpulse / timeStep;
					contactInfo.normal = pt.m_normalWorldOnB;

					if(obj0)
					{
						contactInfo.pos = pt.getPositionWorldOnA();
						obj0->contactWith(obj1, contactInfo);
					}

					if(obj1)
					{
						contactInfo.pos = pt.getPositionWorldOnB();
						obj1->contactWith(obj0, contactInfo);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
