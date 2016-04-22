//---------------------------------------------------------------------------

#ifndef GROUND_OBJECT_H
#define GROUND_OBJECT_H

//---------------------------------------------------------------------------

#include <physics/Physics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	link_class(PlaneObject, Class<PhysicalObject>);

	class PlaneObject : public PhysicalObject
	{
	public:
		PlaneObject(Scene * scene, PhysicalWorld * world, double level) :
			PhysicalObject(
				scene,
				world,
				new btStaticPlaneShape(btVector3(0, 1.0, 0), level),
				{0.0, 0.0, 0.0, 1.0}
				),
			_level(level)
		{
			setclass(PlaneObject);
		}

		virtual ~PlaneObject() {}

	protected:
		double _level = 0;
	};
}

//---------------------------------------------------------------------------
#endif
