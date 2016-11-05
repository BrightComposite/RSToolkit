//---------------------------------------------------------------------------

#pragma once

#ifndef CAMERA_H
#define CAMERA_H

//---------------------------------------------------------------------------

#include "Scene.h"

//---------------------------------------------------------------------------

namespace asd
{
	enum class ProjectionMode
	{
		Ortho,
		Perspective
	};

	class Camera : public Oriented, public SceneObject
	{
	public:
		Camera(Scene * scene) : SceneObject(scene)
		{
			if(scene->camera() == nullptr)
				scene->setCamera(this);

			updateProjection();
		}

		virtual api(scene) void setRotation(const quaternion & rot) override;
		virtual api(scene) void rotate(const quaternion & rot) override;
		virtual api(scene) void move(const vector & offset) override;

		scalar pitch() const
		{
			return _angles[0];
		}

		scalar yaw() const
		{
			return _angles[1];
		}

		scalar roll() const
		{
			return _angles[2];
		}

		virtual api(scene) void setPitch(scalar value);
		virtual api(scene) void setYaw(scalar value);
		virtual api(scene) void setRoll(scalar value);
		virtual api(scene) void setAngles(scalar pitch, scalar yaw, scalar roll);

		virtual api(scene) void addPitch(scalar value);
		virtual api(scene) void addYaw(scalar value);
		virtual api(scene) void addRoll(scalar value);
		virtual api(scene) void addAngles(scalar pitch, scalar yaw, scalar roll);

		scalar viewRange() const
		{
			return _range;
		}

		scalar fieldOfView() const
		{
			return _fov;
		}

		const matrix & projectionMatrix() const
		{
			return _projectionMatrix;
		}

		const matrix & viewMatrix() const
		{
			return _viewMatrix;
		}

		matrix normalMatrix(const matrix & model) const
		{
			return (_viewMatrix * model).invert();
		}

		ProjectionMode projectionMode() const
		{
			return _projectionMode;
		}

		api(scene) void setProjectionMode(ProjectionMode mode);

		api(scene) void setViewRange(scalar range);
		api(scene) void setFieldOfView(scalar fov);

		api(scene) void updateProjection();
		api(scene) void updateView();

	protected:
		api(scene) void updateNormalMatrix();

		scalar _range = 0.01f;
		scalar _fov = 90.0f;
		Rotation _pitch;
		Rotation _yaw;
		Rotation _roll;
		scalar3  _angles = {0.0f, 0.0f, 0.0f};
		ProjectionMode _projectionMode = ProjectionMode::Ortho;
		AlignedMatrix<scalar> _projectionMatrix;
		AlignedMatrix<scalar> _viewMatrix;
	};
}

//---------------------------------------------------------------------------
#endif
