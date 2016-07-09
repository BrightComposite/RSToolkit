//---------------------------------------------------------------------------

#include <ui/UISpace.h>
#include <windows/PointAdapter.h>
#include <windows/RectAdapter.h>
#include <windows/ComException.h>

#include <application/Application.h>

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(UISpace);

	static void getWindowClass(WNDCLASSEXW & wcex, const wchar_t * className, WNDPROC wndProc)
	{
		auto hInstance = Application::getWindowsInstance();

		if(GetClassInfoExW(hInstance, className, &wcex) != FALSE)
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

	UISpace::UISpace(Graphics * graphics, const IntSize & size, HWND handle) : _handle(handle), _width(size.x), _height(size.y), _graphics(graphics)
	{
		setclass(UISpace);

		_surface = graphics->createSurface(this);
		_graphics->bind(_surface);

		_root.init(this, region());
		_focused = _focusList.end();

		_cursor = Cursor::default();
	}

	UISpace::~UISpace()
	{
		_focused = _focusList.end();
		send<UIDestroyMessage>(this);
		_root = nullptr;
	}

	void UISpace::invalidate()
	{
		if(!_invalids.empty())
			_invalids.clear();

		_invalids.emplace_back(0, 0, _width, _height);
	}

	void UISpace::invalidate(const Widget * w)
	{
		if(!w->isVisible())
			return;

		IntRect rect = w->absRegion();

		for(auto i = _invalids.begin(); i != _invalids.end(); ++i)
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

	void UISpace::validate()
	{
		if(_invalids.empty() || !_enabled)
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

	void UISpace::setCursor(Cursor * cursor)
	{
		if(_cursor == cursor)
			return;

		if(_cursor != nullptr)
			_cursor->unbind(this);

		_cursor = cursor;

		if(_cursor != nullptr)
			_cursor->bind(this);
	}

	void UISpace::enable()
	{
		if(_enabled)
			return;

		_enabled = true;
		EnableWindow(_handle, TRUE);
	}

	void UISpace::disable()
	{
		if(!_enabled)
			return;

		_enabled = false;
		EnableWindow(_handle, FALSE);
	}

	Widget * UISpace::focused() const
	{
		return _focused != _focusList.end() ? *_focused : nullptr;
	}

	Widget * UISpace::focusNext()
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

	Widget * UISpace::focusPrevious()
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
	
	const IntPoint & UISpace::cursorPos() const
	{
		return _cursorPos;
	}

	void UISpace::setCursorPos(const IntPoint & pt)
	{
		_cursorPos = pt;
		PointAdapter a(_cursorPos);
		ClientToScreen(_handle, &a);
		SetCursorPos(a.x(), a.y());
	}

	void UISpace::acquireCursorPos(IntPoint & pt) const
	{
		PointAdapter a;
		GetCursorPos(&a);
		ScreenToClient(_handle, &a);

		a.assignTo(pt);
	}

	void UISpace::clipCursor(const IntRect & region)
	{
		_clipRect = region;
		updateClipRect();
		_clipped = true;
	}

	void UISpace::updateClipRect()
	{
		RectAdapter a(_clipRect);

		ClientToScreen(_handle, &a.leftTop());
		ClientToScreen(_handle, &a.rightBottom());
		ClipCursor(&a);

		acquireCursorPos(_cursorPos);
	}

	void UISpace::unclipCursor()
	{
		ClipCursor(nullptr);
		_clipped = false;
	}

	void UISpace::read(Handle<KeyDownMessage> & msg)
	{
		if(!_enabled)
			return;

		Widget * f = focused();

		if(f != nullptr)
			resend(msg, *f);
	}

	void UISpace::read(Handle<CharMessage> & msg)
	{
		if(!_enabled)
			return;

		Widget * f = focused();

		if(f != nullptr)
			resend(msg, *f);
	}

	void UISpace::read(Handle<KeyUpMessage> & msg)
	{
		if(!_enabled)
			return;

		Widget * f = focused();

		if(f != nullptr)
			resend(msg, *f);
	}

	void UISpace::read(Handle<MouseDownMessage> & msg)
	{
		if(!_enabled)
			return;

		_mouseState.press(msg->button);
		send<MouseUpdateMessage>(*this, msg->x, msg->y);

		if(_pointed != nullptr)
		{
			_pressedList.insert({msg->button, _pointed});
			_pointed->_mouseState.press(msg->button);

			int x = msg->x;
			int y = msg->y;

			msg->x -= _pointed->absLeft();
			msg->y -= _pointed->absTop();

			resend(msg, *_pointed);
			send<WidgetPressMessage>(*_pointed, _mouseState.buttons());
			send<WidgetChangedStateMessage>(*_pointed, WidgetState::Pressed, true);

			if(msg->button == MouseButton::Left)
				_pointed->focus();

			msg->x = x;
			msg->y = y;
		}
	}

	void UISpace::read(Handle<MouseUpdateMessage> & msg)
	{
		if(!_enabled)
			return;

		MouseButton buttons = MouseButton::None;

		if(hi_bit_mask::state(GetKeyState(VK_LBUTTON)))
			set_flag(MouseButton::Left, buttons);

		if(hi_bit_mask::state(GetKeyState(VK_MBUTTON)))
			set_flag(MouseButton::Middle, buttons);

		if(hi_bit_mask::state(GetKeyState(VK_RBUTTON)))
			set_flag(MouseButton::Right, buttons);

		unpress(MouseButton(~buttons), msg->x, msg->y, 0);

		Widget * w = _root->findAt({msg->x, msg->y});

		if(w != _pointed)
		{
			if(_pointed != nullptr)
			{
				send<MouseLeaveMessage>(*_pointed);
				send<WidgetChangedStateMessage>(*_pointed, WidgetState::Pointed, false);

				if(_mouseState.isPressed())
				{
					buttons = MouseButton(_pointed->pressedButtons() & _mouseState.buttons());

					if(buttons != MouseButton::None)
					{
						_pointed->_mouseState.unpress(buttons);
						send<WidgetStopPressMessage>(*_pointed, buttons);
						send<WidgetChangedStateMessage>(*_pointed, WidgetState::Pressed, false);
					}
				}
			}

			if(w != nullptr)
			{
				send<MouseEnterMessage>(*w);
				send<WidgetChangedStateMessage>(*w, WidgetState::Pointed, true);

				MouseButton buttons = MouseButton::None;

				for(auto i = _pressedList.begin(); i != _pressedList.end(); ++i)
				{
					if(i->second == w)
						set_flag(i->first, buttons);
				}

				if(buttons != MouseButton::None)
				{
					w->_mouseState.press(buttons);
					send<WidgetPressMessage>(*w, buttons);
					send<WidgetChangedStateMessage>(*w, WidgetState::Pressed, true);
				}
			}
		}

		_pointed = w;

		IntPoint oldPos = _cursorPos;
		_cursorPos = {msg->x, msg->y};
		bool mouseMoved = (_cursorPos - oldPos) != IntPoint{0, 0};

		if(mouseMoved)
			send<MouseMoveMessage>(*this, msg->x, msg->y);

		if(_pointed != nullptr)
		{
			int x = msg->x;
			int y = msg->y;

			msg->x -= _pointed->absLeft();
			msg->y -= _pointed->absTop();

			resend(msg, *_pointed);

			if(mouseMoved)
				send<MouseMoveMessage>(*_pointed, msg->x, msg->y);

			msg->x = x;
			msg->y = y;
		}
	}

	void UISpace::read(Handle<MouseUpMessage> & msg)
	{
		if(!_enabled)
			return;

		send<MouseUpdateMessage>(*this, msg->x, msg->y);
	}

	void UISpace::read(Handle<UIResizeMessage> & msg)
	{
		if(!_enabled)
			return;

		_root->changeSize(_width, _height, ModelMask::FullSize);
	}

	void UISpace::unpress(MouseButton buttons, int x, int y, int flags)
	{
		_mouseState.unpress(buttons);

		for(auto i = _pressedList.begin(); i != _pressedList.end(); ++i)
		{
			if(!check_flag(i->first, buttons))
				continue;

			MouseButton button = i->first;
			Widget * w = i->second;
			i = _pressedList.erase(i);

			w->_mouseState.unpress(button);

			send<MouseUpMessage>(*w, button, x - w->absLeft(), y - w->absTop(), flags);

			if(w == _pointed)
			{
				send<WidgetReleaseMessage>(*w, button);
				send<WidgetChangedStateMessage>(*w, WidgetState::Pressed, false);
			}

			if(i == _pressedList.end())
				break;
		}
	}
}

//---------------------------------------------------------------------------
