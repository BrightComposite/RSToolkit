//---------------------------------------------------------------------------

#include <ui/Panel.h>
#include <ui/Window.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_reader(Panel, WidgetChangedStateMessage)
	{
		_adapter->invalidate(this);
	}
}