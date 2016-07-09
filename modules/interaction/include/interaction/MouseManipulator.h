//---------------------------------------------------------------------------

#pragma once

#ifndef MOUSE_MANIPULATOR_H
#define MOUSE_MANIPULATOR_H

//---------------------------------------------------------------------------

#include <ui/Widget.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class MouseManipulator : public WidgetComponent
	{
	public:
		api(interaction) MouseManipulator(Widget * w);
		virtual ~MouseManipulator() {}

		api(interaction) void enable();
		api(interaction) void disable();
		virtual api(interaction) void toggle();

	protected:
		api(interaction) void onMouseMove(Handle<MouseMoveMessage> &, UISpace &);

		bool _enabled;
		IntPoint _initial;
	};

	class MouseDragger : public MouseManipulator
	{
	public:
		api(interaction) MouseDragger(Widget * w);
		virtual ~MouseDragger() {}

	protected:
		api(interaction) void onMouseDown(Handle<MouseDownMessage> &, Widget &);
	};
}

//---------------------------------------------------------------------------
#endif
