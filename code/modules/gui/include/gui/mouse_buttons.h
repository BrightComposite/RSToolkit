//---------------------------------------------------------------------------

#pragma once

#ifndef MOUSE_BUTTONS_H
#define MOUSE_BUTTONS_H

//---------------------------------------------------------------------------

#include <meta/macro.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace gui
    {
        enum class mouse_button : int
        {
            none = 0,

            left = 1 << 0,
            middle = 1 << 1,
            right = 1 << 2,

            all = (int)left | (int)middle | (int)right,

            any = ~(int)none,

            not_left = ~(int)left,
            not_middle = ~(int)middle,
            not_right = ~(int)right,

            some = ~(int)all
        };

        adapt_enum_flags(mouse_button);

        class mouse_state
        {
        public:
            mouse_button buttons() const {
                return _buttons;
            }

            bool is_pressed() const {
                return _buttons != mouse_button::none;
            }

            bool is_pressed(mouse_button button) const {
                return check_flag(button, _buttons);
            }

            bool has_pressed(mouse_button buttons) const {
                return has_some_flags(buttons, _buttons);
            }

            void press(mouse_button button) {
                set_flag(button, _buttons);
            }

            void unpress(mouse_button button) {
                clear_flag(button, _buttons);
            }

            void unpress() {
                _buttons = mouse_button::none;
            }

        protected:
            mouse_button _buttons = mouse_button::none;
        };
    }
}

//---------------------------------------------------------------------------
#endif
