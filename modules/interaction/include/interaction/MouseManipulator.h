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

		FloatPoint fetchData()
		{
			_wait = false;
			return _delta;
		}

		api(interaction) void enable();
		api(interaction) void disable();
		virtual api(interaction) void toggle();

	protected:
		api(interaction) void onMouseUpdate(Handle<MouseUpdateMessage> &, UISpace &);

		bool _enabled = false;
		bool _wait = false;

		IntPoint _initial;
		FloatPoint _delta = {};
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
