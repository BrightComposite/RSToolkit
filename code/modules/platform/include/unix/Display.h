//---------------------------------------------------------------------------

#pragma once

#ifndef DISPLAY_H
#define DISPLAY_H

//---------------------------------------------------------------------------

#include <meta/Macro.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#undef None

//---------------------------------------------------------------------------

namespace asd
{
	class Display
	{
	public:
		api(platform)
		int dispatchMessages();
	
	private:
		::Display * _display;
	};
}

//---------------------------------------------------------------------------
#endif
