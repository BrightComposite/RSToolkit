//---------------------------------------------------------------------------

#include <ui/ui_space.h>

#if BOOST_OS_WINDOWS
#include <windows/PointAdapter.h>
#include <windows/RectAdapter.h>
#include <windows/ComException.h>
#endif

#include <application/application.h>

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace asd
{
#ifdef WIN32
	static void getWindowClass(WNDCLASSEXW & wcex, const wchar_t * className, WNDPROC wndProc)
	{
		auto hInstance = application::windows_instance();

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

	HWND createWindowHandle(const int_rect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc)
	{
		WNDCLASSEXW wcex;
		getWindowClass(wcex, windowClass.c_str(), wndProc);

		DWORD style = WS_OVERLAPPEDWINDOW;

		RectAdapter a(rect);
		AdjustWindowRect(&a.rect, style, FALSE);

		int_rect r;
		a.assignTo(r);

		if(r.top < 0)
			r.moveTo(r.left, 0);

		HWND hWnd = CreateWindowExW(0, wcex.lpszClassName, caption.c_str(), style,
			r.left, r.top, r.width(), r.height(), nullptr, nullptr, wcex.hInstance, nullptr);

		if(!hWnd)
			throw Exception("Can't create window (", caption, ")");

		return hWnd;
	}

	static LRESULT CALLBACK empty_wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if(message == WM_CREATE)
			return 0;

		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	HWND createEmptyWindowHandle()
	{
		WNDCLASSEXW wcex;
		getWindowClass(wcex, L"asdEmptyWindowClass", empty_wnd_proc);

		static int windowCounter = 0;
		WideString caption("empty", windowCounter++);

		HWND hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, wcex.lpszClassName, caption.c_str(), WS_SIMPLE,
			0, 0, 1, 1, nullptr, nullptr, wcex.hInstance, nullptr);

		if(!hWnd)
			throw Exception("Can't create empty window (", caption, ")");

		return hWnd;
	}
	
	ui_space::ui_space(asd::graphics * graphics, const int_size & size, HWND handle) : _handle(handle), _width(size.x), _height(size.y), _graphics(graphics) {
		setclass(ui_space);
		
		_surface = graphics->createSurface(this);
		_graphics->bind(_surface);
		
		_root.init(this, region());
		_focused = _focusList.end();
		
		//_cursor = Cursor::default();
	}
#elif BOOST_OS_LINUX
	ui_space::ui_space(asd::graphics * graphics, const int_size & size) : _width(size.x), _height(size.y), _graphics(graphics) {
		_display = graphics->display();
		
		if(_display == nullptr) {
			throw Exception("Can't open X display");
		}
		
		auto vinfo = graphics->visualInfo();
		auto root = DefaultRootWindow(_display);
		
		XSetWindowAttributes swa;
		swa.colormap = XCreateColormap(_display, root, vinfo->visual, AllocNone);
		swa.event_mask = ExposureMask | KeyPressMask;
		
		_handle = XCreateWindow(_display, root, 0, 0, 600, 600, 0, vinfo->depth, InputOutput, vinfo->visual, CWColormap | CWEventMask, &swa);
		
		if(_handle == 0) {
			throw Exception("Can't create X window");
		}
		
		pool()[_handle] = this;
		
		_wm.DELETE_WINDOW = XInternAtom(_display, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(_display, _handle, &_wm.DELETE_WINDOW, 1);
		
		_surface = graphics->createSurface(this);
		_graphics->bind(_surface);
		
		_root.init(this, region());
		_focused = _focusList.end();
		
		//_cursor = Cursor::default();
	}
#endif
	
	ui_space::~ui_space() {
		_focused = _focusList.end();
		send<UIDestroyMessage>(this);
		_root = nullptr;
		
#if BOOST_OS_LINUX
		if(_display != nullptr) {
			if(_handle != 0) {
				pool().erase(_handle);
				XDestroyWindow(_display, _handle);
				_handle = 0;
			}
			
			XCloseDisplay(_display);
		}
#endif
	}
	
	void ui_space::invalidate() {
		if(!_invalids.empty()) {
			_invalids.clear();
		}
		
		_invalids.emplace_back(0, 0, _width, _height);
	}
	
	void ui_space::invalidate(const widget * w) {
		if(!w->isVisible()) {
			return;
		}
		
		int_rect rect = w->absRegion();
		
		for(auto i = _invalids.begin(); i != _invalids.end(); ++i) {
			auto & r = *i;
			
			if(rect.intersects(r)) {
				r.include(rect);
				return;
			}
			
			if(rect.area() > r.area()) {
				_invalids.insert(i, rect);
				return;
			}
		}
		
		_invalids.push_back(rect);
	}
	
	void ui_space::validate() {
		if(_invalids.empty() || !_enabled) {
			return;
		}
		
		_graphics->bind(_surface);
		
		for(auto & rect : _invalids) {
			_graphics->clip(rect);
			_graphics->surface()->clear();
			
			for(auto & ch : _root->_displayList) {
				ch->draw(_graphics, rect);
			}
		}
		
		_invalids.clear();
		_graphics->present();
	}

#ifdef WIN32
	void ui_space::setCursor(Cursor * cursor)
	{
		if(_cursor == cursor)
			return;

		if(_cursor != nullptr)
			_cursor->unbind(this);

		_cursor = cursor;

		if(_cursor != nullptr)
			_cursor->bind(this);
	}

	void ui_space::showCursor()
	{
		ShowCursor(true);
	}

	void ui_space::hideCursor()
	{
		ShowCursor(false);
	}
	
	void ui_space::enable()
	{
		if(_enabled)
			return;

		_enabled = true;
		EnableWindow(_handle, TRUE);
	}

	void ui_space::disable()
	{
		if(!_enabled)
			return;

		_enabled = false;
		EnableWindow(_handle, FALSE);
	}
#else
	
	void ui_space::enable() {
		if(_enabled) {
			return;
		}
		
		_enabled = true;
	}
	
	void ui_space::disable() {
		if(!_enabled) {
			return;
		}
		
		_enabled = false;
	}

#endif
	
	widget * ui_space::focused() const {
		return _focused != _focusList.end() ? *_focused : nullptr;
	}
	
	widget * ui_space::focusNext() {
		if(_focusList.size() == 0) {
			return nullptr;
		}
		
		widget * w = focused();
		
		if(_focused == _focusList.end()) {
			--_focused;
		}
		
		do {
			++_focused;
			
			if(_focused == _focusList.end()) {
				_focused = _focusList.begin();
			}
			
			if(w == focused()) {
				return w;
			}
		} while(!(*_focused)->isVisible());
		
		if(w != nullptr) {
			w->removeFocus();
		}
		
		(*_focused)->receiveFocus();
		
		return *_focused;
	}
	
	widget * ui_space::focusPrevious() {
		if(_focusList.size() == 0) {
			return nullptr;
		}
		
		widget * w = focused();
		
		do {
			if(_focused == _focusList.begin()) {
				_focused = _focusList.end();
			}
			
			--_focused;
			
			if(w == focused()) {
				return w;
			}
		} while(!(*_focused)->isVisible());
		
		if(w != nullptr) {
			w->removeFocus();
		}
		
		(*_focused)->receiveFocus();
		
		return *_focused;
	}
	
	const int_point & ui_space::cursorPos() const {
		return _cursorPos;
	}
	
	void ui_space::mouseUpdate() {
		if(!_active) {
			return;
		}
		
		int_point pt;
		acquireCursorPos(pt);
		send<MouseUpdateMessage>(*this, pt.x, pt.y);
	}
	
	void ui_space::clipCursor(const int_rect & region) {
		_cursorClipRect = region;
		updateCursorClipRect();
	}

#ifdef WIN32
	void ui_space::setCursorPos(const int_point & pt)
	{
		_cursorPos = pt;

		if(!_active)
			return;

		PointAdapter a(_cursorPos);
		ClientToScreen(_handle, &a);
		SetCursorPos(a.x(), a.y());
	}

	void ui_space::acquireCursorPos(int_point & pt) const
	{
		PointAdapter a;
		GetCursorPos(&a);
		ScreenToClient(_handle, &a);

		a.assignTo(pt);
	}
	
	void ui_space::updateCursorClipRect()
	{
		if(!_active)
			return;

		RectAdapter a(_cursorClipRect);

		ClientToScreen(_handle, &a.leftTop());
		ClientToScreen(_handle, &a.rightBottom());

		_clippedCursor = ClipCursor(&a) == TRUE;
		acquireCursorPos(_cursorPos);
	}

	void ui_space::unclipCursor()
	{
		ClipCursor(nullptr);
		_clippedCursor = false;
	}
#else
	
	void ui_space::setCursorPos(const int_point & pt) {
		_cursorPos = pt;
		
		if(!_active) {
			return;
		}
		
		// TODO: implement `setCursorPos`
	}
	
	void ui_space::acquireCursorPos(int_point & pt) const {
		// TODO: implement `acquireCursorPos`
	}
	
	void ui_space::updateCursorClipRect() {
		if(!_active) {
			return;
		}
		
		// TODO: implement `updateCursorClipRect`
		acquireCursorPos(_cursorPos);
	}
	
	void ui_space::unclipCursor() {
		// TODO: implement `unclipCursor`
		_clippedCursor = false;
	}

#endif

#ifdef WIN32
	void ui_space::setBackgroundColor(const rgb & color)
	{
		SetClassLongPtrW(_handle, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(color.r, color.g, color.b)));
	}
#else
	
	void ui_space::setBackgroundColor(const rgb & color) {
		// TODO: implement `setBackgroundColor`
	}

#endif
	
	void ui_space::read(asd::handle<KeyDownMessage> & msg) {
		if(!_enabled) {
			return;
		}
		
		widget * f = focused();
		
		if(f != nullptr) {
			this->resend(msg, *f);
		}
	}
	
	void ui_space::read(asd::handle<CharMessage> & msg) {
		if(!_enabled) {
			return;
		}
		
		widget * f = focused();
		
		if(f != nullptr) {
			this->resend(msg, *f);
		}
	}
	
	void ui_space::read(asd::handle<KeyUpMessage> & msg) {
		if(!_enabled) {
			return;
		}
		
		widget * f = focused();
		
		if(f != nullptr) {
			this->resend(msg, *f);
		}
	}
	
	void ui_space::read(asd::handle<MouseDownMessage> & msg) {
		if(!_enabled) {
			return;
		}
		
		_mouseState.press(msg->button);
		this->send<MouseUpdateMessage>(*this, msg->x, msg->y);
		
		if(_pointed != nullptr) {
			_pressedList.insert({msg->button, _pointed});
			_pointed->_mouseState.press(msg->button);
			
			int x = msg->x;
			int y = msg->y;
			
			msg->x -= _pointed->absLeft();
			msg->y -= _pointed->absTop();
			
			this->resend(msg, *_pointed);
			this->send<WidgetPressMessage>(*_pointed, _mouseState.buttons());
			this->send<WidgetChangedStateMessage>(*_pointed, WidgetState::Pressed, true);
			
			if(msg->button == mouse_button::left) {
				_pointed->focus();
			}
			
			msg->x = x;
			msg->y = y;
		}
	}
	
	void ui_space::read(asd::handle<MouseMoveMessage> & msg) {
		if(!_enabled) {
			return;
		}
		
		_cursorPos = {msg->x, msg->y};
		widget * w = _root->findAt(_cursorPos);
		
		if(w != _pointed) {
			if(_pointed != nullptr) {
				this->send<MouseLeaveMessage>(*_pointed);
				this->send<WidgetChangedStateMessage>(*_pointed, WidgetState::Pointed, false);
				
				if(_mouseState.is_pressed()) {
					mouse_button buttons = mouse_button(_pointed->pressedButtons() & _mouseState.buttons());
					
					if(buttons != mouse_button::none) {
						_pointed->_mouseState.unpress(buttons);
						this->send<WidgetStopPressMessage>(*_pointed, buttons);
						this->send<WidgetChangedStateMessage>(*_pointed, WidgetState::Pressed, false);
					}
				}
			}
			
			if(w != nullptr) {
				this->send<MouseEnterMessage>(*w);
				this->send<WidgetChangedStateMessage>(*w, WidgetState::Pointed, true);
				
				mouse_button buttons = mouse_button::none;
				
				for(auto i = _pressedList.begin(); i != _pressedList.end(); ++i) {
					if(i->second == w) {
						set_flag(i->first, buttons);
					}
				}
				
				if(buttons != mouse_button::none) {
					w->_mouseState.press(buttons);
					this->send<WidgetPressMessage>(*w, buttons);
					this->send<WidgetChangedStateMessage>(*w, WidgetState::Pressed, true);
				}
			}
		}
		
		_pointed = w;
		
		if(_pointed != nullptr) {
			int x = msg->x;
			int y = msg->y;
			
			msg->x -= _pointed->absLeft();
			msg->y -= _pointed->absTop();
			
			this->resend(msg, *_pointed);
			
			msg->x = x;
			msg->y = y;
		}
	}
	
	void ui_space::read(asd::handle<MouseUpdateMessage> & msg) {
		if(!_enabled) {
			return;
		}
		
		mouse_button buttons = mouse_button::none;
		#ifdef WIN32
		if(hi_bit_mask::state(GetKeyState(VK_LBUTTON))) {
			set_flag(mouse_button::left, buttons);
		}
		
		if(hi_bit_mask::state(GetKeyState(VK_MBUTTON))) {
			set_flag(mouse_button::middle, buttons);
		}
		
		if(hi_bit_mask::state(GetKeyState(VK_RBUTTON))) {
			set_flag(mouse_button::right, buttons);
		}
		#else
		// TODO: implement mouse buttons update stuff
		#endif
		
		unpress(mouse_button(~buttons), msg->x, msg->y, 0);
		
		if(_cursorPos != int_point{msg->x, msg->y}) {
			this->send<MouseMoveMessage>(*this, msg->x, msg->y);
		}
		
		if(_pointed != nullptr) {
			int x = msg->x;
			int y = msg->y;
			
			msg->x -= _pointed->absLeft();
			msg->y -= _pointed->absTop();
			
			this->resend(msg, *_pointed);
			
			msg->x = x;
			msg->y = y;
		}
	}
	
	void ui_space::read(asd::handle<MouseUpMessage> & msg) {
		if(!_enabled) {
			return;
		}
		
		this->send<MouseUpdateMessage>(*this, msg->x, msg->y);
	}
	
	void ui_space::read(asd::handle<UIMoveMessage> & msg) {
		if(_clippedCursor) {
			updateCursorClipRect();
		} else {
			acquireCursorPos(_cursorPos);
		}
	}
	
	void ui_space::read(asd::handle<UIResizeMessage> & msg) {
		if(!_enabled) {
			return;
		}
		
		_root->changeSize(_width, _height, ModelMask::FullSize);
	}
	
	void ui_space::unpress(mouse_button buttons, int x, int y, int flags) {
		_mouseState.unpress(buttons);
		
		for(auto i = _pressedList.begin(); i != _pressedList.end(); ++i) {
			if(!check_flag(i->first, buttons)) {
				continue;
			}
			
			mouse_button button = i->first;
			widget * w = i->second;
			i = _pressedList.erase(i);
			
			w->_mouseState.unpress(button);
			
			this->send<MouseUpMessage>(*w, button, x - w->absLeft(), y - w->absTop(), flags);
			
			if(w == _pointed) {
				this->send<WidgetReleaseMessage>(*w, button);
				this->send<WidgetChangedStateMessage>(*w, WidgetState::Pressed, false);
			}
			
			if(i == _pressedList.end()) {
				break;
			}
		}
	}
}

//---------------------------------------------------------------------------
