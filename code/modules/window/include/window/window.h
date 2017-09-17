//---------------------------------------------------------------------------

#pragma once

#ifndef WINDOW_H
#define WINDOW_H

//---------------------------------------------------------------------------

#include <boost/predef.h>
#include <graphics/graphics.h>

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
	using WindowHandle = HWND;
#elif BOOST_OS_LINUX
	using WindowHandle = ::Window;
#endif
	
	class window;
	
	class window_context
	{
	public:
		window_context(window & w) : _window(w) {}
		virtual ~window_context() {}
		
		virtual void prepare() = 0;
#if BOOST_OS_WINDOWS

#elif BOOST_OS_LINUX
		XVisualInfo * visualInfo() const
		{
			return _visualInfo;
		}
#endif
	
	protected:
		window & _window;
		XVisualInfo * _visualInfo;
	};
	
	exception_class(WindowCreationException);

#if BOOST_OS_LINUX
	class OpenDisplayException : public WindowCreationException
	{
	public:
		OpenDisplayException(const char * display_name) : WindowCreationException("Can't open X display ", display_name, "!") {}
	};
#endif
	
	class window
	{
	public:
		/**
		 * @brief
		 * 	window constructor
		 *
		 * @tparam Gfx
		 *
		 * @param driver
		 * @param displayName
		 *
		 * @throws WindowCreationException|OpenDisplayException|GraphicsException
		 */
		template <class Gfx>
		window(Gfx & driver, const char * displayName = nullptr);
		
		virtual ~window();
		
		WindowHandle handle() const {
			return _handle;
		}
		
		gfx::context & graphics() const {
			return *_graphics;
		}

#if BOOST_OS_WINDOWS

#elif BOOST_OS_LINUX
		::Display * display() const {
			return _display;
		}
#endif
	
	private:
		WindowHandle _handle;
		unique<gfx::context> _graphics;
		
#if BOOST_OS_WINDOWS

#elif BOOST_OS_LINUX
		::Display * _display;
#endif
	};
	
	template <class Gfx>
	window::window(Gfx & driver, const char * displayName) {
#if BOOST_OS_WINDOWS

#elif BOOST_OS_LINUX
		_display = XOpenDisplay(displayName);
		
		if(_display == nullptr) {
			throw OpenDisplayException(displayName);
		}
		
		auto screen = DefaultScreen(_display);
		auto root = RootWindow(_display, screen);
		
		auto context = driver.create_context(*this);
		auto * windowContext = static_cast<window_context *>(context.get());
		_graphics = std::move(context);
		
		auto * visualInfo = windowContext->visualInfo();
		
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
		
		XMapWindow(_display, _handle);
		XStoreName(_display, _handle, "ASD");
		
		windowContext->prepare();
#endif
	}
}

//---------------------------------------------------------------------------
#endif
