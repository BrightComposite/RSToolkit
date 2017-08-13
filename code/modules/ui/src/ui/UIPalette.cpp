//---------------------------------------------------------------------------

#include <ui/UIPalette.h>
#include <ui/Text.h>

//---------------------------------------------------------------------------

namespace asd
{
	StandartUIPalette::StandartUIPalette(ui_space * space) : UIPalette(space)
	{
		handle<widget> button(_space, int_rect {0, 0, 120, 80});
		handle<widget> label(_space, int_rect {0, 0, 120, 80});
		Text::setDynamic(label, "", Font::obtain("Arial", "arial.ttf"));

		_classes["button"].init("button", button);
		_classes["label"].init("label", label);
	}

	widget * StandartUIPalette::create(const string & classid, widget * parent)
	{
		auto & cl = _classes[classid];

		if(cl == nullptr)
			return nullptr;

		auto obj = cl->prototype->clone(parent);
		obj->properties->set(cl);
		return obj;
	}

	void StandartUIPalette::add(const string & classid, const handle<widget> & prototype)
	{
		auto & cl = _classes[classid];

		if(cl != nullptr)
			throw Exception("Standart UI palette: Widget prototype for class \"", classid, "\" already exists");

		cl.init(classid, prototype);
	}
}

//---------------------------------------------------------------------------
