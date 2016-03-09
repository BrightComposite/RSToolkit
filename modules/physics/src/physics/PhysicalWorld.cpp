//---------------------------------------------------------------------------

#include <physics/PhysicalWorld.h>
#include <physics/PhysicalObject.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	void PhysicalWorld::update(btDynamicsWorld * world, btScalar timeStep)
	{
		auto * w = static_cast<PhysicalWorld *>(world->getWorldUserInfo());

		int num = world->getDispatcher()->getNumManifolds();
		ContactInfo contactInfo;

		for(int i = 0; i < num; i++)
		{
			auto * manifold = world->getDispatcher()->getManifoldByIndexInternal(i);

			auto * obj0 = static_cast<PhysicalObject *>(manifold->getBody0() ? manifold->getBody0()->getUserPointer() : nullptr);
			auto * obj1 = static_cast<PhysicalObject *>(manifold->getBody1() ? manifold->getBody1()->getUserPointer() : nullptr);

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
