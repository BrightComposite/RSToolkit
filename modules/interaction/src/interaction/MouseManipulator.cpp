//---------------------------------------------------------------------------

#include <interaction/MouseManipulator.h>
#include <ui/UISpace.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	static const IntPoint center {5, 5};

	MouseManipulator::MouseManipulator(Widget * w) : WidgetComponent(w)
	{
		connect(this, &MouseManipulator::onMouseUpdate, *w->space());
	}

	void MouseManipulator::onMouseUpdate(Handle<MouseUpdateMessage> & msg, UISpace & space)
	{
		if(_wait || !_enabled)
			return;

		IntPoint current {msg->x, msg->y};
		_delta = current - _old;

		if(_delta != FloatPoint{})
		{
			//IntPoint s = sqr(current - center);

			//if(s.x + s.y > sqr(center.x / 2))
			//{
				current = center;
				_widget->space()->setCursorPos(center);
			//}

			_old = current;

			_delta.x = fmath::clamp(_delta.x, -center.x * 0.4f, center.x * 0.4f);
			_delta.y = fmath::clamp(_delta.y, -center.y * 0.4f, center.y * 0.4f);
			
			_delta.x = fmath::pow(_delta.x / center.x, 3) * center.x;
			_delta.y = fmath::pow(_delta.y / center.y, 3) * center.y;
		}
	}

	void MouseManipulator::enable()
	{
		if(_enabled)
			return;

		toggle();
	}

	void MouseManipulator::disable()
	{
		if(!_enabled)
			return;

		toggle();
	}
	
	void MouseManipulator::toggle()
	{
		if(!_enabled)
		{
			_widget->space()->hideCursor();
			_initial = _widget->space()->cursorPos();
			_delta = {};
			_wait = true;
			_old = center;
			_widget->space()->setCursorPos(center);
			_widget->space()->clipCursor({IntPoint{0, 0}, center * 2 + IntPoint{1, 1}});
		}
		else
		{
			_delta = {};
			_widget->space()->unclipCursor();
			_widget->space()->setCursorPos(_initial);
			_widget->space()->showCursor();
		}

		_enabled = !_enabled;
	}

	MouseDragger::MouseDragger(Widget * w) : MouseManipulator(w)
	{
		connect(this, &MouseDragger::onMouseDown, *w);
	}

	void MouseDragger::onMouseDown(Handle<MouseDownMessage> &, Widget &)
	{

	}
}

//---------------------------------------------------------------------------
