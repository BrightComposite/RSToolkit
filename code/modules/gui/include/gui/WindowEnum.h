//---------------------------------------------------------------------------

#pragma once

#ifndef WINDOW_ENUM_H
#define WINDOW_ENUM_H

//---------------------------------------------------------------------------

namespace asd
{
	enum class WindowState
	{
		Normal, Hidden, Minimized, Maximized
	};
	
	enum class BorderStyle
	{
		Normal, Static, Fixed, None
	};
}

//---------------------------------------------------------------------------
#endif
