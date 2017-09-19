//---------------------------------------------------------------------------

#pragma once

#ifndef WINDOW_H
#define WINDOW_H

//---------------------------------------------------------------------------

#include <boost/predef.h>
#include <graphics/graphics.h>
#include <math/rect.h>

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
#if BOOST_OS_WINDOWS
	using window_handle_t = HWND;
#elif BOOST_OS_LINUX
	using window_handle_t = ::Window;
#endif
	
	class window;
	
	class window_context
	{
	public:
		window_context(window & w) : _window(w) {}
		virtual ~window_context() {}

		virtual void prepare() = 0;

#if BOOST_OS_LINUX
		XVisualInfo * visualInfo() const
		{
			return _visualInfo;
		}
#endif
	
	protected:
		window & _window;

#if BOOST_OS_LINUX
		XVisualInfo * _visualInfo;
#endif
	};

	exception_class(WindowCreationException);

#if BOOST_OS_LINUX
	class OpenDisplayException : public WindowCreationException
	{
	public:
		OpenDisplayException(const char * display_name) : WindowCreationException("Can't open X display ", display_name, "!") {}
	};
#endif

#if BOOST_OS_WINDOWS
	#define WS_SIMPLE (WS_POPUP | WS_CLIPCHILDREN | WS_MINIMIZEBOX)
#endif

	class window
	{
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
		window(const string & caption, const math::int_rect & rect, const char * displayName = nullptr);
		
		api(window)
		virtual ~window();
		
		window_handle_t handle() const {
			return _handle;
		}
		
		template<class Gfx>
		inline gfx::driver_context<Gfx> & bind(Gfx & driver);

		gfx::context & graphics() const {
			BOOST_ASSERT_MSG(_context, "There is not graphic context bound!");
			return *_context;
		}

#if BOOST_OS_WINDOWS
		HDC device() const {
			return _device;
		}
#elif BOOST_OS_LINUX
		::Display * display() const {
			return _display;
		}
#endif

		api(window)
		void show();

		api(window)
		void hide();

#if BOOST_OS_WINDOWS
		api(window)
		static HWND create_handle(const math::int_rect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc);

		api(window)
		static HWND create_empty_handle();
#endif

	private:
		window_handle_t _handle;
		unique<gfx::context> _context;
		
#if BOOST_OS_WINDOWS
		HDC _device;
#elif BOOST_OS_LINUX
		::Display * _display;
#endif
	};

#if BOOST_OS_WINDOWS

	template<class Gfx>
	inline gfx::driver_context<Gfx> & window::bind(Gfx & driver) {
		auto tmp = driver.create_context(*this);
		auto ctx = tmp.get();
		_context = std::move(tmp);

		ctx->prepare();
		return *ctx;
	}

#elif BOOST_OS_LINUX

	template<class Gfx>
	inline gfx::driver_context<Gfx> & window::bind(Gfx & driver) {
		auto screen = DefaultScreen(_display);
		auto root = RootWindow(_display, screen);

		auto tmp = driver.create_context(*this);
		auto ctx = tmp.get();

		_context = std::move(tmp);

		auto * visualInfo = _context->visualInfo();

		{
			XSetWindowAttributes winAttr;

			winAttr.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask;
			winAttr.background_pixmap = 0;
			winAttr.background_pixel = 0;
			winAttr.border_pixel = 0;

			winAttr.colormap = XCreateColormap(_display, root, visualInfo->visual, AllocNone);

			unsigned int mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;

			_handle = XCreateWindow(_display, root, 0, 0, 640, 480, 0, visualInfo->depth, InputOutput, visualInfo->visual, mask, &winAttr);
		}

		if(!_handle) {
			throw WindowCreationException("Cannot create X Window!");
		}

		XStoreName(_display, _handle, "ASD");

		ctx->prepare();

		return *ctx;
	}

#endif
}

//---------------------------------------------------------------------------
#endif
