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
		virtual api(interaction) ~MouseManipulator();

		api(interaction) FloatPoint fetchData();

		api(interaction) void enable();
		api(interaction) void disable();
		virtual api(interaction) void toggle();

	protected:
		api(interaction) void callback(Handle<MouseMoveMessage> &, UISpace &);

		bool _enabled = false;
		bool _wait = false;

		IntPoint _initial;
		FloatPoint _current = {};
		IntPoint _old = {};
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
