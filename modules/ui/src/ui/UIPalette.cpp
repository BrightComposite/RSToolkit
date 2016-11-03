//---------------------------------------------------------------------------

#include <ui/UIPalette.h>
#include <ui/Text.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	StandartUIPalette::StandartUIPalette(UISpace * space) : UIPalette(space)
	{
		Handle<Widget> button(_space, IntRect {0, 0, 120, 80});
		Handle<Widget> label(_space, IntRect {0, 0, 120, 80});
		Text::setDynamic(label, "", Font::obtain("Arial", "arial.ttf"));

		_collection["button"] = button;
		_collection["label"] = label;
	}

	Widget * StandartUIPalette::create(const string & classid, Widget * parent)
	{
		auto & w = _collection[classid];

		if(w == nullptr)
			return nullptr;

		return w->clone(parent);
	}

	void StandartUIPalette::add(const string & classid, const Handle<Widget> & prototype)
	{
		auto & w = _collection[classid];

		if(w != nullptr)
			throw Exception("Standart UI palette: Widget prototype for class \"", classid, "\" already exists");

		w = prototype;
	}
}

//---------------------------------------------------------------------------
