//---------------------------------------------------------------------------

#include <ui/UIPalette.h>
#include <ui/Text.h>

//---------------------------------------------------------------------------

namespace asd
{
	StandartUIPalette::StandartUIPalette(UISpace * space) : UIPalette(space)
	{
		Handle<Widget> button(_space, IntRect {0, 0, 120, 80});
		Handle<Widget> label(_space, IntRect {0, 0, 120, 80});
		Text::setDynamic(label, "", Font::obtain("Arial", "arial.ttf"));

		_classes["button"].init("button", button);
		_classes["label"].init("label", label);
	}

	Widget * StandartUIPalette::create(const string & classid, Widget * parent)
	{
		auto & cl = _classes[classid];

		if(cl == nullptr)
			return nullptr;

		auto obj = cl->prototype->clone(parent);
		obj->properties->set(cl);
		return obj;
	}

	void StandartUIPalette::add(const string & classid, const Handle<Widget> & prototype)
	{
		auto & cl = _classes[classid];

		if(cl != nullptr)
			throw Exception("Standart UI palette: Widget prototype for class \"", classid, "\" already exists");

		cl.init(classid, prototype);
	}
}

//---------------------------------------------------------------------------
