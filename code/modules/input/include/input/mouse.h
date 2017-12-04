//---------------------------------------------------------------------------

#pragma once

#ifndef MOUSE_H
#define MOUSE_H

//---------------------------------------------------------------------------

#include <meta/types.h>

//---------------------------------------------------------------------------


namespace asd
{
	enum class mouse_button : uint
	{
		none   			= 0,
		
		left   			= 1 << 0,
		middle 			= 1 << 1,
		right  			= 1 << 2,
		x1	   			= 1 << 3,
		x2	   			= 1 << 4,
		
		any   			= ~uint(none),
		
		not_left   		= ~uint(left),
		not_middle 		= ~uint(middle),
		not_right  		= ~uint(right),
		not_x1 			= ~uint(x1),
		not_x2  		= ~uint(x2),
	};
	
	adapt_enum_flags(mouse_button);
	
	using mouse_buttons = flags<mouse_button>;
}

//---------------------------------------------------------------------------
#endif
