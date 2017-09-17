//---------------------------------------------------------------------------

#include <window/window.h>

//---------------------------------------------------------------------------

namespace asd
{

#if BOOST_OS_WINDOWS

#elif BOOST_OS_LINUX
	
	window::~window() {
		if(_display != nullptr) {
			if(_handle != 0) {
				XDestroyWindow(_display, _handle);
				_handle = 0;
			}
			
			XCloseDisplay(_display);
			_display = nullptr;
		}
	}
	
#endif
}

//---------------------------------------------------------------------------
