//---------------------------------------------------------------------------

#pragma once

#ifndef UI_PALETTE_H
#define UI_PALETTE_H

//---------------------------------------------------------------------------

#include <ui/UISpace.h>

//---------------------------------------------------------------------------

namespace asd
{
	class UIClass : public WidgetProperty
	{
	public:
		UIClass(const string & id, const Handle<Widget> & prototype) : id(id), prototype(prototype) {}

		string id;
		Handle<Widget> prototype;
	};

	create_property(ui, UIClass);

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

		template<class F, useif<is_callable<F, Widget *>::value>>
		void foreach(F functor)
		{
			_space->root()->foreach(functor);
		}

	protected:
		UISpace * _space;
		Dictionary<string, UIClass> _classes;
	};

	class StandartUIPalette : public UIPalette
	{
	public:
		api(ui) StandartUIPalette(UISpace * space);
		virtual ~StandartUIPalette() {}

		virtual api(ui) Widget * create(const string & classid, Widget * parent = nullptr) override;

		api(ui) void add(const string & classid, const Handle<Widget> & prototype);
	};
}

//---------------------------------------------------------------------------
#endif
