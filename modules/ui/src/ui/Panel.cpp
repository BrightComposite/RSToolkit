//---------------------------------------------------------------------------

#include <ui/Panel.h>
#include <ui/Window.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	msg_implement_reader(Panel, WidgetChangedStateMessage)
	{
		_adapter->invalidate(this);
	}
}