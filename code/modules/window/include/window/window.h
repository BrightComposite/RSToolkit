//---------------------------------------------------------------------------

#pragma once

#ifndef WINDOW_H
#define WINDOW_H

//---------------------------------------------------------------------------

#include <boost/predef.h>
#include <observable/observable.hpp>
#include <graphics/graphics.h>
#include <math/rect.h>

#define observable_class OBSERVABLE_PROPERTIES

#if BOOST_OS_WINDOWS
#include <windows.h>
#elif BOOST_OS_LINUX

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <core/Exception.h>

#endif

//---------------------------------------------------------------------------

namespace asd
{
	using namespace observable;
	
	class window;
	
	exception_class(window_creation_exception, "Can't create window");

#if BOOST_OS_WINDOWS
	
	using window_handle_t = HWND;

	class window_context
	{
	public:
		window_context(window & w) : _window(w) {}
		
		virtual ~window_context() {}
		
		virtual void prepare() = 0;
	
	protected:
		window & _window;
	};

#define WS_SIMPLE (WS_POPUP | WS_CLIPCHILDREN | WS_MINIMIZEBOX)

#elif BOOST_OS_LINUX
	
	using window_handle_t = ::Window;
	
	class window_context
	{
	public:
		window_context(window & w) : _window(w) {}
        window_context(window_context && ctx) : _window(ctx._window), _visual_info(ctx._visual_info) {
            ctx._visual_info = nullptr;
        }
		
		virtual ~window_context() {}
		
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
	
	class window
	{
		observable_class(window);
		deny_copy(window);
		
	public:
		/**
		 * @brief
		 * 	window constructor
		 *
		 * @param caption
		 * @param rect
		 * @param displayName
		 *
		 * @throws WindowCreationException|OpenDisplayException|GraphicsException
		 */
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
		template <class Gfx>
		inline gfx::driver_context<Gfx> & bind(Gfx & driver);
		
		gfx::context & graphics() const {
			BOOST_ASSERT_MSG(_context != nullptr, "There is not graphic context bound!");
			return *_context;
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
		int loop(F iteration) {
			while (true) {
				auto result = process_events();
				
				if (result == 0) {
					return 0;
				}
				
				iteration();
			}
		}
		
		observable_property<math::int_point> position;
		observable_property<math::int_size> size;
		observable_property<math::int_rect> area;
		
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

#elif BOOST_OS_LINUX
		
		::Display * _display;
		XWindowAttributes _attributes;
		XEvent _event;
#endif
	};
	
	template <class Gfx>
	inline gfx::driver_context<Gfx> & window::bind(Gfx & driver) {
		auto & ctx = driver.create_context(*this);
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
