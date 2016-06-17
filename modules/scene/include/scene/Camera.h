//---------------------------------------------------------------------------

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

		void api(scene) setProjectionMode(ProjectionMode mode);

		void api(scene) setZoom(float zoom);
		void api(scene) setFieldOfView(float fov);
		void api(scene) updateProjection();

		void api(scene) update();

	protected:
		float _zoom = 0.01f;
		float _fov = 90.0f;
		ProjectionMode _projectionMode = ProjectionMode::Ortho;
	};
}

//---------------------------------------------------------------------------
#endif
