//---------------------------------------------------------------------------

#include <interaction/mouse_manipulator.h>
#include <ui/ui_space.h>

//---------------------------------------------------------------------------

namespace asd
{
	static const IntPoint center {5, 5};

	mouse_manipulator::mouse_manipulator(Widget * w) : WidgetComponent(w)
	{
		connect(this, &mouse_manipulator::callback, *w->space());
	}

	mouse_manipulator::~mouse_manipulator()
	{
		disable();
	}

	FloatPoint mouse_manipulator::fetchData()
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

	void mouse_manipulator::callback(handle<MouseMoveMessage> & msg, UISpace & space)
	{
		_current.set(msg->x, msg->y);
	}

	void mouse_manipulator::enable()
	{
		if(!_enabled)
			toggle();
	}

	void mouse_manipulator::disable()
	{
		if(_enabled)
			toggle();
	}

	void mouse_manipulator::toggle()
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

	MouseDragger::MouseDragger(Widget * w) : mouse_manipulator(w)
	{
		connect(this, &MouseDragger::onMouseDown, *w);
	}

	void MouseDragger::onMouseDown(handle<MouseDownMessage> &, Widget &)
	{

	}
}

//---------------------------------------------------------------------------
