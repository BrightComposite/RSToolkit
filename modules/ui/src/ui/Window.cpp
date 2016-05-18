//---------------------------------------------------------------------------

#include <ui/Window.h>
#include <windows/RectAdapter.h>

#include <windows.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	Window::Window(Graphics * graphics, const IntRect & rect, const WideString & caption) : UISpace(graphics, rect.size(), createWindowHandle(rect, caption, L"RaptureWindowClass", wndProc))
	{
		setclass(Window);

		RectAdapter a;
		GetWindowRect(_handle, &a.rect);
		a.assignTo(_outerRegion);

		_normalStyle = GetWindowLongW(_handle, GWL_STYLE);

		SetWindowLongPtrW(_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		ShowWindow(_handle, SW_HIDE);

		_timer = SetTimer(_handle, 0, 25, nullptr);

	#define HOTKEY_FULLSCREEN 0x20

		registerHotkey(HOTKEY_FULLSCREEN, VK_RETURN, MOD_ALT);

		dest_connect(*this, UISpace, HotkeyMessage)
		{
			auto window = static_cast<Window *>(&dest);

			switch(msg->id)
			{
				case HOTKEY_FULLSCREEN:
					window->toggleFullscreen();
					break;
			}
		};
	}

	Window::~Window() {}

	LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc;

		Window * window = reinterpret_cast<Window *>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

		if(window == nullptr)
			return DefWindowProcW(hWnd, message, wParam, lParam);

		Widget & root = *window->_root.get();

		switch(message)
		{
			case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
				break;
			}

			case WM_CLOSE:
			{
				window->_isClosed = true;
				send<WindowCloseMessage>(*window);
				DestroyWindow(hWnd);
				break;
			}

			case WM_DESTROY:
			{
				KillTimer(hWnd, window->_timer);
				PostQuitMessage(0);
				break;
			}

			case WM_ACTIVATE:
			{
				if(LOWORD(wParam) == 0 && !window->_isClosed)
					window->setFullscreen(false);

				break;
			}

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				switch((int)wParam)
				{
					case VK_TAB:
					{
						if(hi_bit_mask::state(GetKeyState(VK_SHIFT)))
							window->focusPrevious();
						else
							window->focusNext();

						break;
					}
				}

				send<KeyDownMessage>(*window, (int)wParam, (int)(lParam & 0xFFFF), (get_bit<30>(lParam) == 0), (get_bit<24>(lParam) != 0));
				break;
			}

			case WM_CHAR:
			{
				send<CharMessage>(*window, (int)wParam, (int)(lParam & 0xFFFF), (get_bit<30>(lParam) == 0), (get_bit<24>(lParam) != 0));
				break;
			}

			case WM_HOTKEY:
			{
				send<HotkeyMessage>(*window, (int)wParam);
				break;
			}

			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				send<KeyUpMessage>(*window, (int)wParam, (int)(lParam & 0xFFFF), (get_bit<30>(lParam) == 0), (get_bit<24>(lParam) != 0));
				break;
			}

			case WM_LBUTTONDOWN:
			{
				send<MouseDownMessage>(*window, MouseButton::Left, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
				break;
			}

			case WM_MBUTTONDOWN:
			{
				send<MouseDownMessage>(*window, MouseButton::Middle, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
				break;
			}

			case WM_RBUTTONDOWN:
			{
				send<MouseDownMessage>(*window, MouseButton::Right, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
				break;
			}

			case WM_LBUTTONUP:
			{
				window->_mouseState.unpress(MouseButton::Left);
				send<MouseUpMessage>(*window, MouseButton::Left, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
				break;
			}

			case WM_MBUTTONUP:
			{
				window->_mouseState.unpress(MouseButton::Middle);
				send<MouseUpMessage>(*window, MouseButton::Middle, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
				break;
			}

			case WM_RBUTTONUP:
			{
				window->_mouseState.unpress(MouseButton::Right);
				send<MouseUpMessage>(*window, MouseButton::Right, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
				break;
			}

			case WM_LBUTTONDBLCLK:
			{
				send<MouseDblClickMessage>(root, MouseButton::Left, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
				break;
			}

			case WM_MBUTTONDBLCLK:
			{
				send<MouseDblClickMessage>(root, MouseButton::Middle, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
				break;
			}

			case WM_RBUTTONDBLCLK:
			{
				send<MouseDblClickMessage>(root, MouseButton::Right, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
				break;
			}

			case WM_MOUSEWHEEL:
			{
				send<MouseWheelMessage>(root, GET_WHEEL_DELTA_WPARAM(wParam), (int)LOWORD(lParam), (int)HIWORD(lParam), (int)LOWORD(wParam));
				break;
			}

			case WM_TIMER:
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(hWnd, &pt);

				send<MouseUpdateMessage>(*window, pt.x, pt.y);
				break;
			}

			case WM_WINDOWPOSCHANGED:
			{
				if(!window->_isShown)
					break;

				WINDOWPOS & pos = *(WINDOWPOS *)lParam;
				window->_outerRegion.setPlacement(pos.x, pos.y, pos.cx, pos.cy);

				RectAdapter a;
				GetClientRect(window->_handle, &a.rect);

				WINDOWPLACEMENT wp;
				GetWindowPlacement(hWnd, &wp);

				if(wp.showCmd == SW_SHOWNORMAL || window->_width != a.width() || window->_height != a.height())
				{
					window->_width = a.width();
					window->_height = a.height();

					send<UIResizeMessage>(*window, window->_width, window->_height);
				}

				if(check_flag(SWP_HIDEWINDOW, pos.flags))
				{
					window->_state = WindowState::Hidden;
					break;
				}

				switch(wp.showCmd)
				{
				case SW_HIDE:
					window->_state = WindowState::Hidden;
					break;

				case SW_MAXIMIZE:
					window->_state = WindowState::Maximized;
					break;

				case SW_MINIMIZE:
				case SW_SHOWMINIMIZED:
				case SW_SHOWMINNOACTIVE:
					window->_state = WindowState::Minimized;
					break;

				case SW_RESTORE:
				case SW_SHOWNORMAL:
					window->_state = WindowState::Normal;
					break;

				case SW_SHOW:
					break;
				}

				break;
			}

			default:
				return DefWindowProcW(hWnd, message, wParam, lParam);
		}

		return 0;
	}

	static void getMonitorSize(int & width, int & height)
	{
		DEVMODE devMode = {0};

		devMode.dmSize = sizeof(DEVMODE);

		EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &devMode);

		width = devMode.dmPelsWidth;
		height = devMode.dmPelsHeight;
	}

	void Window::setFullscreen(bool fullscreen)
	{
		if(_fullscreen != fullscreen)
			toggleFullscreen();
	}

	void Window::makeFullscreen()
	{
		_windowedPlacement.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(_handle, &_windowedPlacement);

		getMonitorSize(_width, _height);
		_outerRegion.set(0, 0, _width, _height);

		send<UIResizeMessage>(*this, _width, _height);
		send<UIFullscreenMessage>(*this, true);

		SetWindowLongW(_handle, GWL_STYLE, WS_CAPTION);
		SetWindowLongW(_handle, GWL_STYLE, WS_POPUP | WS_MINIMIZEBOX);
		SetWindowPos(_handle, HWND_TOP, 0, 0, _width, _height, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

		_fullscreenPlacement.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(_handle, &_fullscreenPlacement);
	}

	void Window::restoreSize()
	{
		send<UIFullscreenMessage>(*this, false);

		SetWindowLongW(_handle, GWL_STYLE, WS_CAPTION);
		SetWindowLongW(_handle, GWL_STYLE, _normalStyle);
		SetWindowPos(_handle, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

		SetWindowPlacement(_handle, &_windowedPlacement);
	}

	void Window::toggleFullscreen()
	{
		_fullscreen = !_fullscreen;

		if(_fullscreen)
			makeFullscreen();
		else
			restoreSize();
	}

	void Window::centralize()
	{
		if(_isShown && (_state != WindowState::Normal || _fullscreen))
			return;

		long w = _outerRegion.width();
		long h = _outerRegion.height();
		long x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
		long y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

		SetWindowPos(_handle, nullptr, x, y, w, h, 0);
		_outerRegion.setPlacement(x, y, w, h);
	}

	void Window::registerHotkey(int id, int key, int modifiers)
	{
		RegisterHotKey(_handle, id, modifiers, key);
	}

	void Window::unregisterHotkey(int id)
	{
		UnregisterHotKey(_handle, id);
	}

	void Window::setBorderStyle(BorderStyle style)
	{
		if(_borderStyle == style)
			return;

		_borderStyle = style;
		applyBorderStyle();
	}

	void Window::applyBorderStyle()
	{
		switch(_borderStyle)
		{
		case BorderStyle::Normal:
			_normalStyle = WS_OVERLAPPEDWINDOW;
			break;
		case BorderStyle::Static:
			_normalStyle = WS_BORDER;
			break;
		case BorderStyle::Fixed:
			_normalStyle = WS_CAPTION;
			break;
		case BorderStyle::None:
			_normalStyle = WS_POPUP | WS_MINIMIZEBOX;
			break;
		}

		SetWindowLongW(_handle, GWL_STYLE, _normalStyle);

		if(_isShown)
			ShowWindow(_handle, SW_SHOW);
	}

	void Window::setCaption(const WideString & caption)
	{
		SetWindowTextW(_handle, caption.c_str());
	}

	WideString Window::getCaption()
	{
		auto length = GetWindowTextLengthW(_handle);
		auto text = Memory<wchar_t>::allocate(length);

		GetWindowTextW(_handle, text, length);

		wstring s(text, length);
		Memory<wchar_t>::free(text);

		return s;
	}

	void Window::setState(WindowState value)
	{
		switch(value)
		{
		case WindowState::Hidden:
			hide();
			break;

		case WindowState::Maximized:
			maximize();
			break;

		case WindowState::Minimized:
			minimize();
			break;

		case WindowState::Normal:
			restore();
			break;
		}
	}

	void Window::show()
	{
		if(_isShown && _state != WindowState::Hidden)
			return;

		_isShown = true;
		SetWindowPos(_handle, nullptr, _outerRegion.left, _outerRegion.top, _outerRegion.width(), _outerRegion.height(), SWP_SHOWWINDOW);
		SetForegroundWindow(_handle);
	}

	void Window::minimize()
	{
		if(!_isShown || _state == WindowState::Minimized)
			return;

		ShowWindow(_handle, SW_MINIMIZE);
	}

	void Window::maximize()
	{
		if(!_isShown || _state == WindowState::Maximized)
			return;

		ShowWindow(_handle, SW_SHOWMAXIMIZED);
	}

	void Window::restore()
	{
		if(!_isShown || _state == WindowState::Normal)
			return;

		ShowWindow(_handle, SW_RESTORE);
	}

	void Window::hide()
	{
		if(!_isShown || _state == WindowState::Hidden)
			return;

		ShowWindow(_handle, SW_HIDE);
	}

	void Window::close()
	{
		SendMessageW(_handle, WM_CLOSE, 0, 0);
	}
}

//---------------------------------------------------------------------------
