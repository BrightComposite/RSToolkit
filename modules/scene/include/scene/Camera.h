//---------------------------------------------------------------------------

#ifndef CAMERA_H
#define CAMERA_H

//---------------------------------------------------------------------------

#include <math/Vector.h>
#include <math/Quaternion.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Scene;

	class Camera : public Shared
	{
	public:
		Camera(Scene * scene) : _scene(scene) {}

		void api(scene) update();

		void api(scene) move(fvec offset);
		void api(scene) move(float x, float y, float z);
		void api(scene) moveX(float x);
		void api(scene) moveY(float y);
		void api(scene) moveZ(float z);

		fvec  position;
		fquat direction;

	protected:
		Scene * _scene;
	};
}

//---------------------------------------------------------------------------
#endif
