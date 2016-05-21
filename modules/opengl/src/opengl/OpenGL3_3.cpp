//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>

#include <ui/UISpace.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace OpenGL3_3
	{
		GLGraphics::GLGraphics()
		{
			initDevice();
			initFacilities();

			_wnd = createEmptyWindowHandle();
			_device = GetDC(_wnd);
			_handle = wglCreateContext(_device);
		}

		GLGraphics::~GLGraphics()
		{
			if(_device != nullptr)
			{
				ReleaseDC(_wnd, _device);
				DeleteDC(_device);
			}

			if(_wnd != nullptr)
				DestroyWindow(_wnd);

			freeFacilities();
		}
	}
}
