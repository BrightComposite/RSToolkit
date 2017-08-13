//---------------------------------------------------------------------------

#include <interaction/MouseManipulator.h>
#include <ui/ui_space.h>

//---------------------------------------------------------------------------

namespace asd
{
	static const IntPoint center {5, 5};

	MouseManipulator::MouseManipulator(Widget * w) : WidgetComponent(w)
	{
		connect(this, &MouseManipulator::callback, *w->space());
	}

	MouseManipulator::~MouseManipulator()
	{
		disable();
	}

	FloatPoint MouseManipulator::fetchData()
	{
		if(!_enabled)
			return {};

		FloatPoint delta = _current - center;
		// remove peaks
		delta.x = fmath::clamp(delta.x, -center.x * 0.8f, center.x * 0.8f);
		delta.y = fmath::clamp(delta.y, -center.y * 0.8f, center.y * 0.8f);

		// increase smoothness
		delta.x = fmath::pow(delta.x / center.x, 3) * center.x;
		delta.y = fmath::pow(delta.y / center.y, 3) * center.y;

		_widget->space()->setCursorPos(center);
		_current = center;

		return delta;
	}

	void MouseManipulator::callback(handle<MouseMoveMessage> & msg, UISpace & space)
	{
		_current.set(msg->x, msg->y);
	}

	void MouseManipulator::enable()
	{
		if(!_enabled)
			toggle();
	}

	void MouseManipulator::disable()
	{
		if(_enabled)
			toggle();
	}

	void MouseManipulator::toggle()
	{
		if(!_enabled)
		{
			_initial = _widget->space()->cursorPos();
			_current = center;
//			_widget->space()->hideCursor();
			_widget->space()->setCursorPos(center);
			_widget->space()->clipCursor({IntPoint{0, 0}, center * 2 + IntPoint{1, 1}});
		}
		else
		{
			_widget->space()->unclipCursor();
			_widget->space()->setCursorPos(_initial);
//			_widget->space()->showCursor();
		}

		_enabled = !_enabled;
	}

	MouseDragger::MouseDragger(Widget * w) : MouseManipulator(w)
	{
		connect(this, &MouseDragger::onMouseDown, *w);
	}

	void MouseDragger::onMouseDown(handle<MouseDownMessage> &, Widget &)
	{

	}
}

//---------------------------------------------------------------------------
