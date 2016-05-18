//---------------------------------------------------------------------------

#ifndef PANEL_H
#define PANEL_H

//---------------------------------------------------------------------------

#include "Widget.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Panel : public Widget
	{
	public:
		Panel(Widget * parent) : Panel(parent, *parent) {}
		Panel(Widget * parent, const IntRect & region) : Widget(parent, region)
		{
			setVisibility(true);
		}

		virtual ~Panel() {}

		virtual bool isDisplayable() const final
		{
			return true;
		}

		override_reader(Panel, WidgetChangedStateMessage);
	};
}

//---------------------------------------------------------------------------
#endif