//---------------------------------------------------------------------------

#pragma once

#ifndef UI_PALETTE_H
#define UI_PALETTE_H

//---------------------------------------------------------------------------

#include <ui/ui_space.h>

//---------------------------------------------------------------------------

namespace asd
{
	class UIClass : public widget_property
	{
	public:
		UIClass(const string & id, const handle<widget> & prototype) : id(id), prototype(prototype) {}

		string id;
		handle<widget> prototype;
	};

	create_property(ui, UIClass);

	class UIPalette : public shareable
	{
	public:
		UIPalette(ui_space * space) : _space(space) {}
		virtual ~UIPalette() {}

		ui_space * space() const
		{
			return _space;
		}

		virtual widget * create(const string & classid, widget * parent = nullptr) = 0;

		template<class F, useif<is_callable<F, widget *>::value>>
		void foreach(F functor)
		{
			for(auto w : _space->root()->tree())
				functor(w);
		}

	protected:
		ui_space * _space;
		Dictionary<string, UIClass> _classes;
	};

	class StandartUIPalette : public UIPalette
	{
	public:
		api(ui) StandartUIPalette(ui_space * space);
		virtual ~StandartUIPalette() {}

		virtual api(ui) widget * create(const string & classid, widget * parent = nullptr) override;

		api(ui) void add(const string & classid, const handle<widget> & prototype);
	};
}

//---------------------------------------------------------------------------
#endif
