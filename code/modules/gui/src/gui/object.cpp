//---------------------------------------------------------------------------

#include <gui/object.h>
#include <gui/space.h>

#include <graphics/graphics.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace gui
    {
        object::object() : _layout(asd::_) {

        }

        object::object(const handle<gui::layout> & layout) : _layout(layout) {

        }

        object::object(const object & w, const handle<gui::layout> & layout) :
            _layout(layout), _area(w._area)
        {

        }

        object::~object() {

        }
    }
}

//---------------------------------------------------------------------------
