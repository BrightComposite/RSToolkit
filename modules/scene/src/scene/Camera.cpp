//---------------------------------------------------------------------------

#include <scene/Camera.h>

//---------------------------------------------------------------------------

namespace Rapture
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
				_scene->graphics().updateUniform<Uniforms::Projection>(floatm::orthot(-1.0f / aspect, 1.0f / aspect, -1.0f, 1.0f, -_range, _range));
				break;
			}

			case ProjectionMode::Perspective:
			{
				_scene->graphics().updateUniform<Uniforms::Projection>(floatm::perspectivet(_fov, aspect, 0.01f, 2 * _range));
				break;
			}
		}
	}

	void Camera::setViewRange(float range)
	{
		_range = range;
	}

	void Camera::setFieldOfView(float fov)
	{
		_fov = fov;
	}

	void Camera::setRotation(const floatq & rot)
	{
		_rot = rot;
	}

	void Camera::rotate(const floatq & rot)
	{
		_rot->rotateBy(rot);
	}

	void Camera::move(const floatv & offset)
	{
		*_pos += _yaw->applyTo(offset);
	}

	void Camera::setPitch(float value)
	{
		_angles->x = fmath::clamp(value, -fmath::half_pi, fmath::half_pi);
		_pitch = fquat(floatv::right, _angles->x);

		_rot = *_roll * *_yaw * *_pitch;
	}

	void Camera::setYaw(float value)
	{
		_angles->y = fmath::rmod(value);
		_yaw = fquat(floatv::up, _angles->y);

		_rot = *_roll * *_yaw * *_pitch;
	}

	void Camera::setRoll(float value)
	{
		_angles->z = value;
		_roll = fquat(floatv::forward, _angles->z);

		_rot = *_roll * *_yaw * *_pitch;
	}

	void Camera::setAngles(float pitch, float yaw, float roll)
	{
		_angles->x = fmath::clamp(pitch, -fmath::pi * 0.49f, fmath::pi * 0.49f);
		_angles->y = fmath::rmod(yaw);
		_angles->z = roll;

		_pitch = fquat(floatv::right, _angles->x);
		_yaw   = fquat(floatv::up, _angles->y);
		_roll  = fquat(floatv::forward, _angles->z);

		_rot = *_roll * *_yaw * *_pitch;
	}

	void Camera::addPitch(float value)
	{
		setPitch(_angles->x + value);
	}

	void Camera::addYaw(float value)
	{
		setYaw(_angles->y + value);
	}

	void Camera::addRoll(float value)
	{
		setRoll(_angles->z + value);
	}

	void Camera::addAngles(float pitch, float yaw, float roll)
	{
		setAngles(_angles->x + pitch, _angles->y + yaw, _angles->z + roll);
	}

	void Camera::update()
	{
		_scene->graphics().updateUniform<Uniforms::View>(floatm::lookTo(_pos, _rot->forward(), _rot->up()).transpose());
	}
}

//---------------------------------------------------------------------------
