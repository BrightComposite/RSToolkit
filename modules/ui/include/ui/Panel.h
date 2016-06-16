//---------------------------------------------------------------------------

#ifndef PANEL_H
#define PANEL_H

//---------------------------------------------------------------------------

#include "Widget.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Panel;

	link_class(ui, Panel, Class<Widget>);

	class Panel : public Widget
	{
	public:
		Panel(Widget * parent) : Panel(parent, *parent) {}
		Panel(Widget * parent, const IntRect & region) : Widget(parent, region)
		{
			setclass(Panel);
			setVisibility(true);
		}

		virtual ~Panel() {}

		virtual bool isDisplayable() const final
		{
			return true;
		}

		using Widget::read;
		virtual void api(ui) read(Handle<WidgetChangedStateMessage> & msg) override;
	};
}

//---------------------------------------------------------------------------
#endif