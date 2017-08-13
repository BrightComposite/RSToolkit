//---------------------------------------------------------------------------

#include <windows/Display.h>
#include <windows.h>

//---------------------------------------------------------------------------

namespace asd
{
	int Display::dispatchMessages() {
		static MSG msg = {0};

		if(PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);

			if(msg.message == WM_QUIT)
				return 2;
		}

		return 0;
	}
}

//---------------------------------------------------------------------------
