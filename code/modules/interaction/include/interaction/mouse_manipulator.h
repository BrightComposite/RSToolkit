//---------------------------------------------------------------------------

#pragma once

#ifndef MOUSE_MANIPULATOR_H
#define MOUSE_MANIPULATOR_H

//---------------------------------------------------------------------------

#include <ui/Widget.h>

//---------------------------------------------------------------------------

namespace asd
{
	class mouse_manipulator : public WidgetComponent
	{
	public:
		api(interaction) mouse_manipulator(Widget * w);
		virtual api(interaction) ~mouse_manipulator();

		api(interaction) FloatPoint fetchData();

		virtual handle<WidgetComponent> clone(Widget * widget) const override
		{
			return handle<mouse_manipulator>(widget);
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

	class MouseDragger : public mouse_manipulator
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
