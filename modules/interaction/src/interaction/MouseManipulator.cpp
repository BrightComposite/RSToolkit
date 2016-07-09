//---------------------------------------------------------------------------

#include <interaction/MouseManipulator.h>
#include <ui/UISpace.h>

//---------------------------------------------------------------------------

namespace Rapture
{

	MouseManipulator::MouseManipulator(Widget * w) : WidgetComponent(w)
	{
		connect(this, onMouseMove, *w->space());
	}

	void MouseManipulator::onMouseMove(Handle<MouseMoveMessage> &, UISpace &)
	{

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
		_enabled = !_enabled;

		if(_enabled)
		{
			_initial = _widget->space()->cursorPos();
			_widget->space()->clipCursor({0, 0, 20, 20});
		}
		else
		{
			_widget->space()->unclipCursor();
			_widget->space()->setCursorPos(_initial);
		}
	}

	MouseDragger::MouseDragger(Widget * w) : MouseManipulator(w)
	{
		connect(this, onMouseDown, *w);
	}

	void MouseDragger::onMouseDown(Handle<MouseDownMessage> &, Widget &)
	{

	}
}

//---------------------------------------------------------------------------
