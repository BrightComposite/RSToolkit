//---------------------------------------------------------------------------

#include <scene/Camera.h>

//---------------------------------------------------------------------------

namespace asd
{
	void Camera::setProjectionMode(ProjectionMode mode)
	{
		if(_projectionMode == mode)
			return;

		_projectionMode = mode;
		updateProjection();
	}

	void Camera::updateProjection()
	{
		float aspect = _scene->viewport().ratio();

		switch(_projectionMode)
		{
			case ProjectionMode::Ortho:
			{
				_projectionMatrix = matrix::orthot(-1.0x / aspect, 1.0x / aspect, -1.0x, 1.0x, -_range, _range);
				break;
			}

			case ProjectionMode::Perspective:
			{
				_projectionMatrix = matrix::perspectivet(_fov, aspect, 0.01x, 2 * _range);
				break;
			}
		}

		_scene->graphics().updateUniform<Uniforms::Projection>(*_projectionMatrix);
	}

	void Camera::setViewRange(scalar range)
	{
		_range = range;
	}

	void Camera::setFieldOfView(scalar fov)
	{
		_fov = fov;
	}

	void Camera::setRotation(const quaternion & rot)
	{
		_rot = rot;
	}

	void Camera::rotate(const quaternion & rot)
	{
		_rot->rotateBy(rot);
	}

	void Camera::move(const vector & offset)
	{
		_pos += _yaw->applyTo(offset);
	}

	void Camera::setPitch(scalar value)
	{
		_angles[0] = math::clamp(value, -math::half_pi, math::half_pi);
		_pitch = Rotation(vector::right, _angles[0]);

		_rot = _roll * _yaw * _pitch;
	}

	void Camera::setYaw(scalar value)
	{
		_angles[1] = math::rmod(value);
		_yaw = Rotation(vector::up, _angles[1]);

		_rot = _roll * _yaw * _pitch;
	}

	void Camera::setRoll(scalar value)
	{
		_angles[2] = value;
		_roll = Rotation(vector::forward, _angles[2]);

		_rot = _roll * _yaw * _pitch;
	}

	void Camera::setAngles(scalar pitch, scalar yaw, scalar roll)
	{
		_angles[0] = math::clamp(pitch, -math::pi * 0.49x, math::pi * 0.49x);
		_angles[1] = math::rmod(yaw);
		_angles[2] = roll;

		_pitch = Rotation(vector::right, _angles[0]);
		_yaw   = Rotation(vector::up, _angles[1]);
		_roll  = Rotation(vector::forward, _angles[2]);

		_rot = _roll * _yaw * _pitch;
	}

	void Camera::addPitch(scalar value)
	{
		setPitch(_angles[0] + value);
	}

	void Camera::addYaw(scalar value)
	{
		setYaw(_angles[1] + value);
	}

	void Camera::addRoll(scalar value)
	{
		setRoll(_angles[2] + value);
	}

	void Camera::addAngles(scalar pitch, scalar yaw, scalar roll)
	{
		setAngles(_angles[0] + pitch, _angles[1] + yaw, _angles[2] + roll);
	}

	void Camera::updateView()
	{
		_viewMatrix = matrix::lookTo(_pos, _rot->forward(), _rot->up()).transpose();
		_scene->graphics().updateUniform<Uniforms::View>(*_viewMatrix);
	}
}

//---------------------------------------------------------------------------
