//---------------------------------------------------------------------------

#pragma once

#ifndef GUI_OBJECT_H
#define GUI_OBJECT_H

//---------------------------------------------------------------------------

#include <container/array_list.h>
#include <meta/class_id.h>
#include <math/rect.h>
#include <math/point.h>
#include <gui/mouse_buttons.h>
#include <graphics/coords.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace gui
    {
        class object;

        using namespace gfx;

        enum class side
        {
            left,
            top,
            right,
            bottom
        };

        class component
        {
            origin_class(component);

        public:
            component(object & widget) : _widget(widget) {}
            virtual ~component() {}

            virtual handle<component> clone(object & widget) const = 0;

        protected:
            object & _widget;
        };

        class layout
        {
        public:
            layout() {}

            virtual ~layout() {}

            virtual void update(gui::area & area) {

            }
        };

        class object : public shareable<object>
        {
            deny_copy(object);

        public:
            api(gui)
            object();
            api(gui)
            object(const handle<gui::layout> & layout);
            api(gui)
            object(const object & w, const handle<gui::layout> & layout);

            api(gui)
            virtual ~object();

            virtual inline handle<object> clone(const handle<gui::layout> & layout) const;

            inline gui::size size() const;
            inline gui::size::coord_type width() const;
            inline gui::size::coord_type height() const;

            inline gui::position pos() const;
            inline coord left() const;
            inline coord top() const;
            inline coord right() const;
            inline coord bottom() const;
            inline coord side(gui::side index) const;

            inline operator const gui::area & () const;
            inline const gui::area & area() const;

            inline void update();

        protected:
            handle<gui::layout> _layout;
            gui::area _area;
            asd::any_list _components;
        };

        inline handle<object> object::clone(const handle<gui::layout> & layout) const
        {
            return make::handle<object>(*this, layout);
        }

        inline gui::size object::size() const
        {
            return _area.size();
        }

        inline gui::size::coord_type object::width() const
        {
            return _area.width();
        }

        inline gui::size::coord_type object::height() const
        {
            return _area.height();
        }

        inline gui::position object::pos() const
        {
            return _area.pos();
        }

        inline coord object::left() const
        {
            return _area.left;
        }

        inline coord object::top() const
        {
            return _area.top;
        }

        inline coord object::right() const
        {
            return _area.right;
        }

        inline coord object::bottom() const
        {
            return _area.bottom;
        }

        inline coord object::side(gui::side index) const
        {
            return _area[static_cast<int>(index)];
        }

        inline object::operator const gui::area & () const
        {
            return _area;
        }

        inline const gui::area & object::area() const
        {
            return _area;
        }
        
        inline void object::update()
        {
            _layout->update(_area);
        }
    }
}

//---------------------------------------------------------------------------
#endif
