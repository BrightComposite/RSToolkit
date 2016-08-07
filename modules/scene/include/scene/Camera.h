//---------------------------------------------------------------------------

#pragma once

#ifndef CAMERA_H
#define CAMERA_H

//---------------------------------------------------------------------------

#include <math/Vector.h>
#include <math/Quaternion.h>

#include "Scene.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	enum class ProjectionMode
	{
		Ortho,
		Perspective
	};

	class Camera : public OrientedObject
	{
	public:
		Camera(Scene * scene) : OrientedObject(scene)
		{
			if(scene->camera() == nullptr)
				scene->setCamera(this);

			updateProjection();
		}

		virtual api(scene) void setRotation(const floatq & rot) override;
		virtual api(scene) void rotate(const floatq & rot) override;
		virtual api(scene) void move(const floatv & offset) override;

		float pitch() const
		{
			return _angles->x;
		}

		float yaw() const
		{
			return _angles->y;
		}

		float roll() const
		{
			return _angles->z;
		}

		virtual api(scene) void setPitch(float value);
		virtual api(scene) void setYaw(float value);
		virtual api(scene) void setRoll(float value);
		virtual api(scene) void setAngles(float pitch, float yaw, float roll);

		virtual api(scene) void addPitch(float value);
		virtual api(scene) void addYaw(float value);
		virtual api(scene) void addRoll(float value);
		virtual api(scene) void addAngles(float pitch, float yaw, float roll);

		float zoom() const
		{
			return _zoom;
		}

		float fieldOfView() const
		{
			return _fov;
		}

		ProjectionMode projectionMode() const
		{
			return _projectionMode;
		}

		api(scene) void setProjectionMode(ProjectionMode mode);

		api(scene) void setZoom(float zoom);
		api(scene) void setFieldOfView(float fov);
		api(scene) void updateProjection();

		api(scene) void update();

	protected:
		float _zoom = 0.01f;
		float _fov = 90.0f;
		fquat _pitch;
		fquat _yaw;
		fquat _roll;
		fvec  _angles = {0.0f, 0.0f, 0.0f};
		ProjectionMode _projectionMode = ProjectionMode::Ortho;
	};
}

//---------------------------------------------------------------------------
#endif
