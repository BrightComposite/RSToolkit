//---------------------------------------------------------------------------

#include <ui/Panel.h>
#include <ui/Window.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(Panel);

	void Panel::read(Handle<WidgetChangedStateMessage> & msg)
	{
		_space->invalidate(this);
	}
}