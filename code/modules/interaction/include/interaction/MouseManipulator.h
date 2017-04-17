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

		virtual Handle<WidgetComponent> clone(Widget * widget) const override
		{
			return Handle<MouseManipulator>(widget);
		}

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

		virtual Handle<WidgetComponent> clone(Widget * widget) const override
		{
			return Handle<MouseDragger>(widget);
		}

	protected:
		api(interaction) void onMouseDown(Handle<MouseDownMessage> &, Widget &);
	};
}

//---------------------------------------------------------------------------
#endif
