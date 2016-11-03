//---------------------------------------------------------------------------

#pragma once

#ifndef UI_PALETTE_H
#define UI_PALETTE_H

//---------------------------------------------------------------------------

#include <ui/Widget.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class UIPalette : public Shared
	{
	public:
		UIPalette(UISpace * space) : _space(space) {}
		virtual ~UIPalette() {}

		UISpace * space() const
		{
			return _space;
		}

		virtual Widget * create(const string & classid, Widget * parent = nullptr) = 0;

	protected:
		UISpace * _space;
	};

	class StandartUIPalette : public UIPalette
	{
	public:
		api(ui) StandartUIPalette(UISpace * space);
		virtual ~StandartUIPalette() {}

		virtual api(ui) Widget * create(const string & classid, Widget * parent = nullptr) override;

		api(ui) void add(const string & classid, const Handle<Widget> & prototype);

	protected:
		Dictionary<string, Widget> _collection;
	};
}

//---------------------------------------------------------------------------
#endif
