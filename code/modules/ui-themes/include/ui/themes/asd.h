//---------------------------------------------------------------------------

#pragma once

#ifndef ASD_UI_THEME_H
#define ASD_UI_THEME_H

//---------------------------------------------------------------------------

#include <ui/UITheme.h>

//---------------------------------------------------------------------------

namespace asd
{
	class AsdUITheme : public UITheme
	{
	public:
		virtual api(uithemes) void apply(UIPalette * palette) override;
	};
}

//---------------------------------------------------------------------------
#endif
