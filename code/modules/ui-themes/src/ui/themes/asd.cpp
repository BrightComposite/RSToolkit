//---------------------------------------------------------------------------

#include <ui/themes/asd.h>
#include <ui/WidgetLayers.h>
#include <ui/UISpace.h>

//---------------------------------------------------------------------------

namespace asd
{
	void AsdUITheme::apply(UIPalette * palette)
	{
		ColoredButtonDecorator decorator;

		decorator
			.background({0.4f, 0.4f, 0.4f})
			.pressed({0.1f, 0.1f, 0.1f})
			.hovered({0.6f, 0.6f, 0.6f});

		for(auto & w : palette->space()->root()->tree())
		{
		}
	}
}

//---------------------------------------------------------------------------
