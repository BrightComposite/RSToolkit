//---------------------------------------------------------------------------

#ifndef WIDGET_ENUM_H
#define WIDGET_ENUM_H

//---------------------------------------------------------------------------

#include <meta/Macro.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	enum class MouseButton : int
	{
		None   = 0,
		Left   = 1 << 0,
		Middle = 1 << 1,
		Right  = 1 << 2,

		LeftMiddle  = Left   | Middle,
		LeftRight   = Left   | Right,
		MiddleRight = Middle | Right,

		All = Left | Middle | Right,

		NotNone   = ~None,
		NotLeft   = ~Left,
		NotMiddle = ~Middle,
		NotRight  = ~Right,

		NotLeftMiddle  = ~LeftMiddle,
		NotLeftRight   = ~LeftRight,
		NotMiddleRight = ~MiddleRight,

		NotAll = ~All
    };

	enum class ModelMask : int
	{
		Custom = 0,

		Left	= 1 << 0,
		Top		= 1 << 1,
		Right	= 1 << 2,
		Bottom	= 1 << 3,

		LeftTop		= Left  | Top,
		LeftBottom	= Left  | Bottom,
		RightTop	= Right | Top,
		RightBottom	= Right | Bottom,

		Horizontal	= Left | Right,
		Vertical	= Top  | Bottom,

		LeftVertical	 = Left   | Vertical,
		TopHorizontal	 = Top    | Horizontal,
		RightVertical	 = Right  | Vertical,
		BottomHorizontal = Bottom | Horizontal,

		FullSize = Horizontal | Vertical
    };

	enum class WidgetFlag
	{
		Visible = 1 << 0,
		Focusable = 1 << 1
	};

	enum class WidgetState
	{
		Pointed = 0,
		Pressed = 1,
		Focused = 2
	};

	adapt_enum_flags(MouseButton);
	adapt_enum_flags(ModelMask);
	adapt_enum_flags(WidgetFlag);
}

//---------------------------------------------------------------------------
#endif
