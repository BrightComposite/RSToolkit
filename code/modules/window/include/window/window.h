//---------------------------------------------------------------------------

#pragma once

#ifndef WINDOW_H
#define WINDOW_H

//---------------------------------------------------------------------------

#include <boost/predef.h>
#include <rxcpp/rx.hpp>
#include <graphics/graphics.h>
#include <math/rect.h>

#if BOOST_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif BOOST_OS_LINUX

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <core/Exception.h>

#endif

//---------------------------------------------------------------------------

namespace asd
{
    class window;

    namespace rx {
        using namespace rxcpp::subjects;
        using namespace rxcpp::operators;
        using rxcpp::observable;
    }

    exception_class(window_creation_exception, "Can't create window");

#if BOOST_OS_WINDOWS

    using window_handle_t = HWND;

    class basic_window_context
    {
    public:
        basic_window_context(window & w) : _window(w) {}

        virtual ~basic_window_context() {}

        virtual void prepare() = 0;

    protected:
        window & _window;
    };

#define WS_SIMPLE (WS_POPUP | WS_CLIPCHILDREN | WS_MINIMIZEBOX)

#elif BOOST_OS_LINUX

    using window_handle_t = ::Window;

    class basic_window_context
    {
    public:
        basic_window_context(window & w) : _window(w) {}
        basic_window_context(window_context && ctx) : _window(ctx._window), _visual_info(ctx._visual_info) {
            ctx._visual_info = nullptr;
        }

        virtual ~basic_window_context() {}

        virtual void prepare() = 0;

        XVisualInfo * visual_info() const {
            return _visual_info;
        }

    protected:
        window & _window;
        XVisualInfo * _visual_info;
    };

    class open_display_exception : public window_creation_exception
    {
    public:
        open_display_exception(const char * display_name) : window_creation_exception("Can't open X display ", display_name, "!") {}
    };

#endif

    template<class Driver>
    class window_context {};

    class window_inputs
    {
    public:
        window_inputs(const rx::observable<math::int_rect> & area) :
            area(area),
            position(area.map([](const auto & r) { return r.pos(); }).distinct_until_changed()),
            size(area.map([](const auto & r) { return r.size(); }).distinct_until_changed())
        {}

        rx::observable<math::int_rect> area;
        rx::observable<math::int_point> position;
        rx::observable<math::uint_size> size;
    };

    class window
    {
        deny_copy(window);

        rx::subject<math::int_rect> _area_changed;

    public:
        /**
         * @brief
         * 	window constructor
         *
         * @param driver
         * @param caption
         * @param rect
         * @param display_name
         *
         * @throws WindowCreationException|OpenDisplayException|GraphicsException
         */
        template <class Gfx, useif<std::is_base_of<basic_window_context, window_context<Gfx>>::value>>
        inline window(Gfx & driver, const string & caption, const math::int_rect & area, const char * display_name = nullptr);

        api(window)
        window(const string & caption, const math::int_rect & area, const char * displayName = nullptr);

        api(window)
        virtual ~window();

        /**
         * @brief handle
         * @return native window handle
         */
        window_handle_t handle() const {
            return _handle;
        }

        /**
         * @brief bind
         *  Set window graphics and return bound graphic context
         * @tparam Gfx
         * @param driver - graphic driver
         * @return bound graphic context
         */
        template <class Gfx, useif<std::is_base_of<basic_window_context, window_context<Gfx>>::value>>
        inline window_context<Gfx> & bind(Gfx & driver);

        gfx::context & graphics() const {
            BOOST_ASSERT_MSG(_context != boost::none, "There is not graphic context bound!");
            return *_context;
        }

        const math::int_rect & area() const {
            return _area;
        }

        math::int_point position() const {
            return _area.pos();
        }

        math::uint_size size() const {
            return _area.size();
        }

        /**
         * @brief show
         * 	Show the window on the screen
         */
        api(window)
        void show();

        /**
         * @brief hide
         * 	Remove window from the screen
         */
        api(window)
        void hide();

        template <class F>
        int context(F iteration) {
            while (true) {
                auto result = process_events();

                if (result == 0) {
                    return 0;
                }

                iteration();
            }
        }

        window_inputs inputs;

#if BOOST_OS_WINDOWS

        HDC device() const {
            return _device;
        }

        api(window)
        static HWND create_handle(const math::int_rect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc);

        api(window)
        static HWND create_empty_handle();

#elif BOOST_OS_LINUX

        ::Display * display() const {
            return _display;
        }

#endif

    private:
        api(window)
        int process_events();

#if BOOST_OS_LINUX
        api(window)
        void init(XVisualInfo * visual_info);
#endif

        window_handle_t _handle;
        boost::optional<gfx::context &> _context;
        math::int_rect _area;

#if BOOST_OS_WINDOWS

        HDC _device;

        friend LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#elif BOOST_OS_LINUX

        ::Display * _display;
        XWindowAttributes _attributes;
        XEvent _event;
#endif
    };

    template <class Gfx, used_t>
    window::window(Gfx & driver, const string & caption, const math::int_rect & area, const char * display_name)
        : window(caption, area, display_name)
    {
        bind(driver);
    }

    template <class Gfx, used_t>
    window_context<Gfx> & window::bind(Gfx & driver)
    {
        auto & ctx = driver.create_context<window_context<Gfx>>(*this);
        _context = ctx;

#if BOOST_OS_LINUX
        init(ctx.visual_info());
#endif

        ctx.prepare();

        return ctx;
    }
}

//---------------------------------------------------------------------------
#endif
