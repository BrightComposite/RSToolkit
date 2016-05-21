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
		Camera(Scene * scene) {}

		void update();

		fvec  position;
		fquat rotation;

	protected:
		Scene * scene;
	};
}

//---------------------------------------------------------------------------
#endif
