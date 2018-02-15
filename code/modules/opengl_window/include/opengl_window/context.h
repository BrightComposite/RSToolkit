//---------------------------------------------------------------------------

#pragma once

#ifndef OPENGL_WINDOW_CONTEXT_H
#define OPENGL_WINDOW_CONTEXT_H

//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <window/window.h>

//---------------------------------------------------------------------------

namespace asd
{
    template<>
    class window_context<opengl::driver> : public opengl::context, public basic_window_context
    {
        using base = opengl::context;

    public:
        api(opengl_window)
        window_context(opengl::driver & driver, window & w);

        window_context(window_context && ctx) noexcept :
            opengl::context(ctx._driver),
            basic_window_context(std::forward<window_context>(ctx)),
            _context(ctx._context)
        {
            ctx._context = 0;
        }

        window_context & operator = (window_context && ctx) noexcept
        {
            std::swap(_context, ctx._context);
            return *this;
        }

        api(opengl_window)
        virtual ~window_context();

        api(opengl_window)
        virtual void flush() override;

        api(opengl_window)
        virtual void prepare() override;

    protected:
        void init_device();

        GLContext _context;
    };
}

//---------------------------------------------------------------------------
#endif
