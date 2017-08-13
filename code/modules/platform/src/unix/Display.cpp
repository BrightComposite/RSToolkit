//---------------------------------------------------------------------------

#include <unix/Display.h>

//---------------------------------------------------------------------------

namespace asd
{
	int Display::dispatchMessages() {
		if(XPending(_display)) {
			XNextEvent(_display, &_event);
			
			if(_event.type == Expose) {
				//XWindowAttributes gwa;
				//XGetWindowAttributes(_display, w, &gwa);
			} else if(_event.type == KeyPress) {
			}
		}
		
		return 0;
	}
}

//---------------------------------------------------------------------------
