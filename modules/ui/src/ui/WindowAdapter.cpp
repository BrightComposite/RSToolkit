//---------------------------------------------------------------------------

#include <ui/WindowAdapter.h>
#include <windows/RectAdapter.h>
#include <windows.h>

#include <application/Application.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	static void getWindowClass(WNDCLASSEXW & wcex, const wchar_t * className, WNDPROC wndProc)
	{
		auto hInstance = Application::getWindowsInstance();

		if(GetClassInfoExW(hInstance, className, &wcex) == TRUE)
			return;

		wcex.cbSize = sizeof(WNDCLASSEXA);
		wcex.style = CS_HREDRAW | CS_VREDRAW; 
		wcex.lpfnWndProc = wndProc; 
		wcex.cbClsExtra = 0; 
		wcex.cbWndExtra = 0; 
		wcex.hInstance = hInstance;
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); 
		wcex.lpszMenuName = nullptr; 
		wcex.lpszClassName = className;
		wcex.hIconSm = nullptr;

		if(!RegisterClassExW(&wcex))
			throw Exception("Can't register window class ", className);
	}

	HWND createWindowHandle(const IntRect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc)
	{
		WNDCLASSEXW wcex;
		getWindowClass(wcex, windowClass.c_str(), wndProc);

		DWORD style = WS_OVERLAPPEDWINDOW;

		RectAdapter a(rect);
		AdjustWindowRect(&a.rect, style, FALSE);

		IntRect r;
		a.assignTo(r);

		if(r.top < 0)
			r.moveTo(r.left, 0);

		HWND hWnd = CreateWindowExW(0, wcex.lpszClassName, caption.c_str(), style,
			r.left, r.top, r.width(), r.height(), nullptr, nullptr, wcex.hInstance, nullptr);

		if(!hWnd)
			throw Exception("Can't create window (", caption, ")");

		return hWnd;
	}

	static LRESULT CALLBACK emptyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if(message == WM_CREATE)
			return 0;

		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	HWND createEmptyWindowHandle()
	{
		WNDCLASSEXW wcex;
		getWindowClass(wcex, L"RaptureEmptyWindowClass", emptyWndProc);

		static int windowCounter = 0;
		WideString caption("empty", windowCounter++);

		HWND hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, wcex.lpszClassName, caption.c_str(), WS_SIMPLE,
			0, 0, 1, 1, nullptr, nullptr, wcex.hInstance, nullptr);

		if(!hWnd)
			throw Exception("Can't create empty window (", caption, ")");

		return hWnd;
	}

	WindowAdapter::WindowAdapter(Graphics * graphics, const IntSize & size, HWND handle) : _handle(handle), _root(), _width(size.x), _height(size.y), _graphics(graphics)
	{
		setclass(WindowAdapter);

		_surface = _graphics->createWindowSurface(this);
		_graphics->bind(_surface);

		_root.reset(new Widget(this, region()));
		_focused = _focusList.end();
	}

	WindowAdapter::~WindowAdapter() {}

	void WindowAdapter::invalidate()
	{
		if(!_invalids.empty())
			_invalids.clear();

		_invalids.emplace_back(0, 0, _width, _height);
	}

	void WindowAdapter::invalidate(const Widget * w)
	{
		if(!w->isVisible())
			return;

		IntRect rect = w->absRegion();

		for(auto & i = _invalids.begin(); i != _invalids.end(); ++i)
		{
			auto & r = *i;

			if(rect.isIntersecting(r))
			{
				r.include(rect);
				return;
			}

			if(rect.area() > r.area())
			{
				_invalids.insert(i, rect);
				return;
			}
		}

		_invalids.push_back(rect);
	}

	void WindowAdapter::validate()
	{
		if(_invalids.empty())
			return;

		_graphics->bind(_surface);

		for(auto & rect : _invalids)
		{
			_graphics->clip(rect);
			_graphics->surface()->clear();

			for(auto & ch : _root->_displayList)
				ch->draw(_graphics, rect);
		}

		_invalids.clear();
		_graphics->present();
	}

	Widget * WindowAdapter::focused() const
	{
		if(_focused == _focusList.end())
			return nullptr;

		return *_focused;
	}

	Widget * WindowAdapter::focusNext()
	{
		if(_focusList.size() == 0)
			return nullptr;

		Widget * w = focused();

		if(_focused == _focusList.end())
			--_focused;

		do
		{
			++_focused;

			if(_focused == _focusList.end())
				_focused = _focusList.begin();

			if(w == focused())
				return w;
		}
		while(!(*_focused)->isVisible());

		if(w != nullptr)
			w->removeFocus();

		(*_focused)->receiveFocus();

		return *_focused;
	}

	Widget * WindowAdapter::focusPrevious()
	{
		if(_focusList.size() == 0)
			return nullptr;

		Widget * w = focused();

		do
		{
			if(_focused == _focusList.begin())
				_focused = _focusList.end();

			--_focused;

			if(w == focused())
				return w;
		}
		while(!(*_focused)->isVisible());

		if(w != nullptr)
			w->removeFocus();

		(*_focused)->receiveFocus();

		return *_focused;
	}

	void WindowAdapter::close()
	{
		SendMessageW(_handle, WM_CLOSE, 0, 0);
	}

	msg_implement_reader(WindowAdapter, KeyDownMessage)
	{
		Widget * f = focused();

		if(f != nullptr)
			send(msg, *f);
	}

	msg_implement_reader(WindowAdapter, CharMessage)
	{
		Widget * f = focused();

		if(f != nullptr)
			send(msg, *f);
	}

	msg_implement_reader(WindowAdapter, KeyUpMessage)
	{
		Widget * f = focused();

		if(f != nullptr)
			send(msg, *f);
	}


	msg_implement_reader(WindowAdapter, MouseDownMessage)
	{
		_mouseState.press(msg->button);
		send(handle<MouseUpdateMessage>(msg->x, msg->y), *this);

		if(_pointed != nullptr)
		{
			_pressedList.insert({msg->button, _pointed});
			_pointed->_mouseState.press(msg->button);

			int x = msg->x;
			int y = msg->y;

			msg->x -= _pointed->absLeft();
			msg->y -= _pointed->absTop();

			send(msg, *_pointed);
			send(handle<WidgetPressMessage>(_mouseState.buttons()), *_pointed);
			send(handle<WidgetChangedStateMessage>(WidgetState::Pressed, true), *_pointed);

			if(msg->button == MouseButton::Left)
				_pointed->focus();

			msg->x = x;
			msg->y = y;
		}
	}

	msg_implement_reader(WindowAdapter, MouseUpdateMessage)
	{
		MouseButton buttons = MouseButton::None;

		if(hi_bit_mask::state(GetKeyState(VK_LBUTTON)))
			set_flag(MouseButton::Left, buttons);

		if(hi_bit_mask::state(GetKeyState(VK_MBUTTON)))
			set_flag(MouseButton::Middle, buttons);

		if(hi_bit_mask::state(GetKeyState(VK_RBUTTON)))
			set_flag(MouseButton::Right, buttons);

		unpress(MouseButton(~buttons), msg->x, msg->y, 0);

		Widget * w = _root->findWidget({msg->x, msg->y});

		if(w != _pointed)
		{
			if(_pointed != nullptr)
			{
				send(handle<MouseLeaveMessage>(), *_pointed);
				send(handle<WidgetChangedStateMessage>(WidgetState::Pointed, false), *_pointed);

				if(_mouseState.isPressed())
				{
					buttons = MouseButton(_pointed->pressedButtons() & _mouseState.buttons());

					if(buttons != MouseButton::None)
					{
						_pointed->_mouseState.unpress(buttons);
						send(handle<WidgetStopPressMessage>(buttons), *_pointed);
						send(handle<WidgetChangedStateMessage>(WidgetState::Pressed, false), *_pointed);
					}
				}
			}

			if(w != nullptr)
			{
				send(handle<MouseEnterMessage>(), *w);
				send(handle<WidgetChangedStateMessage>(WidgetState::Pointed, true), *w);

				MouseButton buttons = MouseButton::None;

				for(auto i = _pressedList.begin(); i != _pressedList.end(); ++i)
				{
					if(i->second == w)
						set_flag(i->first, buttons);
				}

				if(buttons != MouseButton::None)
				{
					w->_mouseState.press(buttons);
					send(handle<WidgetPressMessage>(buttons), *w);
					send(handle<WidgetChangedStateMessage>(WidgetState::Pressed, true), *w);
				}
			}
		}

		_pointed = w;

		if(_pointed != nullptr)
		{
			int x = msg->x;
			int y = msg->y;

			msg->x -= _pointed->absLeft();
			msg->y -= _pointed->absTop();

			send(msg, _pointed);

			msg->x = x;
			msg->y = y;
		}
	}

	msg_implement_reader(WindowAdapter, MouseUpMessage)
	{
		send(handle<MouseUpdateMessage>(msg->x, msg->y), *this);
	}

	msg_implement_reader(WindowAdapter, WindowHotkeyMessage)
	{

	}

	msg_implement_reader(WindowAdapter, WindowResizeMessage)
	{
		_root->changeSize(_width, _height, ModelMask::FullSize);
	}

	msg_implement_reader(WindowAdapter, WindowFullscreenMessage)
	{
		//invalidate();
	}

	msg_implement_reader(WindowAdapter, WindowCloseMessage)
	{

	}

	void WindowAdapter::unpress(MouseButton buttons, int x, int y, int flags)
	{
		_mouseState.unpress(buttons);

		for(auto i = _pressedList.begin(); i != _pressedList.end(); ++i)
		{
			if(!check_flag(i->first, buttons))
				continue;

			Widget * w = i->second;
			w->_mouseState.unpress(i->first);

			send(handle<MouseUpMessage>(i->first, x - w->absLeft(), y - w->absTop(), flags), *w);

			if(w == _pointed)
			{
				send(handle<WidgetStopPressMessage>(i->first), *w);
				send(handle<WidgetChangedStateMessage>(WidgetState::Pressed, false), *w);
			}

			i = _pressedList.erase(i);
			
			if(i == _pressedList.end())
				break;
		}
	}
}

//---------------------------------------------------------------------------