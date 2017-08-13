//---------------------------------------------------------------------------

#pragma once

#ifndef MOUSE_MANIPULATOR_H
#define MOUSE_MANIPULATOR_H

//---------------------------------------------------------------------------

#include <ui/Widget.h>

//---------------------------------------------------------------------------

namespace asd
{
	class MouseManipulator : public WidgetComponent
	{
	public:
		api(interaction) MouseManipulator(Widget * w);
		virtual api(interaction) ~MouseManipulator();

		api(interaction) FloatPoint fetchData();

		virtual handle<WidgetComponent> clone(Widget * widget) const override
		{
			return handle<MouseManipulator>(widget);
		}

		api(interaction) void enable();
		api(interaction) void disable();
		virtual api(interaction) void toggle();

	protected:
		api(interaction) void callback(handle<MouseMoveMessage> &, UISpace &);

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

		virtual handle<WidgetComponent> clone(Widget * widget) const override
		{
			return handle<MouseDragger>(widget);
		}

	protected:
		api(interaction) void onMouseDown(handle<MouseDownMessage> &, Widget &);
	};
}

//---------------------------------------------------------------------------
#endif
