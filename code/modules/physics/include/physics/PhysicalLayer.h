//---------------------------------------------------------------------------

#pragma once

#ifndef PHYSICAL_LAYER_H
#define PHYSICAL_LAYER_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <container/array_list.h>

#include <space/spatial.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//---------------------------------------------------------------------------

namespace asd
{
	class Physical;

	struct alignas(alignof(vector)) ContactInfo
	{
		vector normal;
		vector pos;
		scalar force;
	};

	class PhysicalLayer : public Object, public btDiscreteDynamicsWorld
	{
		deny_copy(PhysicalLayer);

	public:
		PhysicalLayer(Unique<btDispatcher> && dispatcher, Unique<btBroadphaseInterface> && broadphase, Unique<btConstraintSolver> && solver, Unique<btCollisionConfiguration> && config) : btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config), dispatcher(move(dispatcher)), broadphase(move(broadphase)), solver(move(solver)), config(move(config))
		{
			setInternalTickCallback(PhysicalLayer::update, this);
			setGravity({0.0f, -10.0f, 0.0f});
		}

	protected:
		friend_handle;
		using btDiscreteDynamicsWorld::operator new;
		using btDiscreteDynamicsWorld::operator delete;
		using btDiscreteDynamicsWorld::operator new[];
		using btDiscreteDynamicsWorld::operator delete[];

		static api(physics) void update(btDynamicsWorld * world, btScalar timeStep);

		Unique<btDispatcher> dispatcher;
		Unique<btBroadphaseInterface> broadphase;
		Unique<btConstraintSolver> solver;
		Unique<btCollisionConfiguration> config;
	};
}

//---------------------------------------------------------------------------
#endif
