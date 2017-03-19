//---------------------------------------------------------------------------

#pragma once

#ifndef WIDGET_ENUM_H
#define WIDGET_ENUM_H

//---------------------------------------------------------------------------

#include <meta/Macro.h>

//---------------------------------------------------------------------------

namespace asd
{
	enum class MouseButton : int
	{
		None   = 0,

		Left   = 1 << 0,
		Middle = 1 << 1,
		Right  = 1 << 2,

		LeftMiddle  = (int)Left   | (int)Middle,
		LeftRight   = (int)Left   | (int)Right,
		MiddleRight = (int)Middle | (int)Right,

		All = (int)Left | (int)Middle | (int)Right,

		NotNone   = ~(int)None,

		NotLeft   = ~(int)Left,
		NotMiddle = ~(int)Middle,
		NotRight  = ~(int)Right,

		NotLeftMiddle  = ~(int)LeftMiddle,
		NotLeftRight   = ~(int)LeftRight,
		NotMiddleRight = ~(int)MiddleRight,

		NotAll = ~(int)All
    };

	enum class ModelMask : int
	{
		Custom = 0,

		Left	= 1 << 0,
		Top		= 1 << 1,
		Right	= 1 << 2,
		Bottom	= 1 << 3,

		LeftTop		= (int)Left  | (int)Top,
		LeftBottom	= (int)Left  | (int)Bottom,
		RightTop	= (int)Right | (int)Top,
		RightBottom	= (int)Right | (int)Bottom,

		Horizontal	= (int)Left | (int)Right,
		Vertical	= (int)Top  | (int)Bottom,

		LeftVertical	 = (int)Left   | (int)Vertical,
		TopHorizontal	 = (int)Top    | (int)Horizontal,
		RightVertical	 = (int)Right  | (int)Vertical,
		BottomHorizontal = (int)Bottom | (int)Horizontal,

		FullSize = (int)Horizontal | (int)Vertical
    };

	enum class WidgetFlag
	{
		Visible   = 1 << 0,
		Focusable = 1 << 1
	};

	enum class WidgetState
	{
		Normal  = 0,
		Pointed = 1,
		Pressed = 2,
		Focused = 4
	};

	adapt_enum_flags(MouseButton);
	adapt_enum_flags(ModelMask);
	adapt_enum_flags(WidgetFlag);
	adapt_enum_flags(WidgetState);
}

//---------------------------------------------------------------------------
#endif
