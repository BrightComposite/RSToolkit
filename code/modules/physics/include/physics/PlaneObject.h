//---------------------------------------------------------------------------

#pragma once

#ifndef PLANE_OBJECT_H
#define PLANE_OBJECT_H

//---------------------------------------------------------------------------

#include <physics/Physics.h>

//---------------------------------------------------------------------------

namespace asd
{
	class PlaneObject : public Oriented
	{
	public:
		PlaneObject(PhysicalLayer * world, scalar level) :
			Oriented(vector{0.0x, level, 0.0x}, quaternion{vector::up, vector::forward}),
			_shape(btVector3{0.0x, 1.0x, 0.0x}, 0.0x)
		{
			_physical.init(this, world, _shape);
		}

		virtual ~PlaneObject() {}

	protected:
		Unique<btStaticPlaneShape> _shape;
		Unique<Physical> _physical;
	};
}

//---------------------------------------------------------------------------
#endif
