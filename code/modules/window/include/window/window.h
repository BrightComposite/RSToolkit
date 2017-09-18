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
		HDC device() const {
			return _device;
		}
#elif BOOST_OS_LINUX
		::Display * display() const {
			return _display;
		}
#endif

#if BOOST_OS_WINDOWS
		api(window) static HWND create_handle(const math::int_rect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc);
		api(window) static HWND create_empty_handle();
#endif

	private:
		WindowHandle _handle;
		unique<gfx::context> _graphics;
		
#if BOOST_OS_WINDOWS
		HDC _device;
#elif BOOST_OS_LINUX
		::Display * _display;
#endif
	};

#if BOOST_OS_WINDOWS

	struct RectAdapter
	{
		deny_copy(RectAdapter);

		RectAdapter() : rect {0, 0, 0, 0} {}
		RectAdapter(const RECT & r) : rect (r) {}
		RectAdapter(RECT && r) : rect (forward<RECT>(r)) {}

		template<class T>
		RectAdapter(const math::rect<T> & in)
		{
			set(in);
		}

		template<class T>
		RECT & set(const math::rect<T> & in)
		{
			rect.left = static_cast<long>(in.left);
			rect.top = static_cast<long>(in.top);
			rect.right = static_cast<long>(in.right);
			rect.bottom = static_cast<long>(in.bottom);

			return rect;
		}

		template<class T>
		void assignTo(math::rect<T> & in)
		{
			in.left = static_cast<T>(rect.left);
			in.top = static_cast<T>(rect.top);
			in.right = static_cast<T>(rect.right);
			in.bottom = static_cast<T>(rect.bottom);
		}

		long left() const
		{
			return rect.left;
		}

		long top() const
		{
			return rect.top;
		}

		long right() const
		{
			return rect.right;
		}

		long bottom() const
		{
			return rect.bottom;
		}

		long width() const
		{
			return rect.right - rect.left;
		}

		long height() const
		{
			return rect.bottom - rect.top;
		}

		POINT & leftTop()
		{
			return *reinterpret_cast<POINT *>(&rect.left);
		}

		POINT & rightBottom()
		{
			return *reinterpret_cast<POINT *>(&rect.right);
		}

		const POINT & leftTop() const
		{
			return *reinterpret_cast<const POINT *>(&rect.left);
		}

		const POINT & rightBottom() const
		{
			return *reinterpret_cast<const POINT *>(&rect.right);
		}

		RECT * operator & ()
		{
			return &rect;
		}

		const RECT * operator & () const
		{
			return &rect;
		}

		RECT rect;
	};

#endif

	template <class Gfx>
	window::window(Gfx & driver, const char * displayName) {
#if BOOST_OS_WINDOWS

		_handle = createWindowHandle(rect, caption, L"asdWindowClass", wndProc);
		_device = ::GetDC(_handle);

		RectAdapter a;
		GetWindowRect(_handle, &a.rect);
		a.assignTo(_outerRegion);

		_normalStyle = GetWindowLongW(_handle, GWL_STYLE);

		SetWindowLongPtrW(_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		ShowWindow(_handle, SW_HIDE);

		auto context = driver.create_context(*this);
		auto * windowContext = static_cast<window_context *>(context.get());
		_graphics = std::move(context);

		windowContext->prepare();
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
