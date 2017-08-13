//---------------------------------------------------------------------------

#include <physics/PlaneObject.h>
#include <physics/PhysicalLayer.h>
#include <physics/Physics.h>

//---------------------------------------------------------------------------

namespace asd
{
	void MotionState::getWorldTransform(btTransform & trans) const {
		auto p = object->position();
		auto r = object->rotation();
		trans.setOrigin({p.x, p.y, p.z});
		trans.setRotation({r.x, r.y, r.z});
	}
	
	void MotionState::setWorldTransform(const btTransform & trans) {
		object->setRotation(trans.getRotation());
		object->setPosition(trans.getOrigin());
	}
	
	void Physical::setMass(scalar mass) {
		btVector3 inertia;
		_rigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
		_rigidBody->setMassProps(mass, inertia);
	}
	
	void Physical::setLinearVelocity(const Velocity & v) {
		_rigidBody->setLinearVelocity({v->x, v->y, v->z});
		_rigidBody->activate();
	}
	
	Velocity Physical::getLinearVelocity() {
		return _rigidBody->getLinearVelocity();
	}
	
	void PhysicalLayer::update(btDynamicsWorld * world, btScalar timeStep) {
		//auto * w = static_cast<PhysicalLayer *>(world->getWorldUserInfo());
		
		int num = world->getDispatcher()->getNumManifolds();
		ContactInfo info;
		
		for(int i = 0; i < num; i++) {
			auto * manifold = world->getDispatcher()->getManifoldByIndexInternal(i);
			
			if(manifold->getNumContacts() == 0) {
				continue;
			}
			
			auto * a = static_cast<Physical *>(manifold->getBody0() != nullptr ? manifold->getBody0()->getUserPointer() : nullptr);
			auto * b = static_cast<Physical *>(manifold->getBody1() != nullptr ? manifold->getBody1()->getUserPointer() : nullptr);
			
			for(int j = 0; j < manifold->getNumContacts(); j++) {
				auto & pt = manifold->getContactPoint(j);
				
				if(pt.getDistance() <= 0.0) {
					info.force = pt.m_appliedImpulse / timeStep;
					info.normal = pt.m_normalWorldOnB;
					
					if(a != nullptr) {
						info.pos = pt.getPositionWorldOnA();
						a->contactWith(b, info);
					}
					
					if(b != nullptr) {
						info.pos = pt.getPositionWorldOnB();
						b->contactWith(a, info);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
