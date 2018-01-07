//---------------------------------------------------------------------------

#include <window/window.h>
#include <math/rect.h>

//---------------------------------------------------------------------------

namespace asd
{
	struct RectAdapter
	{
		deny_copy(RectAdapter);

		RectAdapter() : rect{ 0, 0, 0, 0 } {}
		RectAdapter(const RECT & r) : rect(r) {}
		RectAdapter(RECT && r) : rect(forward<RECT>(r)) {}

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
		void assign_to(math::rect<T> & in)
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

	static void getWindowClass(WNDCLASSEXW & wcex, const wchar_t * className, WNDPROC wndProc)
	{
		auto hInstance = GetModuleHandle(NULL);

		if (GetClassInfoExW(hInstance, className, &wcex) != FALSE)
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

		if (!RegisterClassExW(&wcex))
			throw Exception("Can't register window class ", className);
	}

	HWND window::create_handle(const math::int_rect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc)
	{
		WNDCLASSEXW wcex;
		getWindowClass(wcex, windowClass.c_str(), wndProc);

		DWORD style = WS_OVERLAPPEDWINDOW;

		RectAdapter a(rect);
		AdjustWindowRect(&a.rect, style, FALSE);

		math::int_rect r;
		a.assign_to(r);

		if (r.top < 0) {
			r.moveTo(r.left, 0);
		}

		HWND hWnd = CreateWindowExW(0, wcex.lpszClassName, caption.c_str(), style, r.left, r.top, r.width(), r.height(), nullptr, nullptr, wcex.hInstance, nullptr);

		if (!hWnd)
			throw Exception("Can't create window (", caption, ")");

		return hWnd;
	}

	static LRESULT CALLBACK empty_wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_CREATE) {
			return 0;
		}

		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	HWND window::create_empty_handle()
	{
		WNDCLASSEXW wcex;
		getWindowClass(wcex, L"ASDEmptyWindowClass", empty_wnd_proc);

		static int windowCounter = 0;
		WideString caption("empty", windowCounter++);

		HWND hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, wcex.lpszClassName, caption.c_str(), WS_SIMPLE, 0, 0, 1, 1, nullptr, nullptr, wcex.hInstance, nullptr);

		if (!hWnd) {
			throw Exception("Can't create empty window (", caption, ")");
		}

		return hWnd;
	}

	LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		PAINTSTRUCT ps;
		HDC hdc;

		window * window_ptr = reinterpret_cast<window *>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

		if (window_ptr == nullptr) {
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}

		window & w = *window_ptr;

		switch (message) {
			case WM_PAINT: {
				hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
				break;
			}

			case WM_CLOSE: {
				/*w._closed = true;

				if(w._clippedCursor)
				ClipCursor(nullptr);

				DestroyWindow(hWnd);*/
				PostQuitMessage(0);
				break;
			}

			/*
			case WM_DESTROY: {
			send<WindowCloseMessage>(w);
			break;
			}

			case WM_ACTIVATE: {
			w._active = LOWORD(wParam) != 0;

			if(LOWORD(wParam) == 0 && !w._closed) {
			w.setFullscreen(false);

			if(w._clippedCursor)
			ClipCursor(nullptr);
			} else {
			if(w._clippedCursor)
			w.updateCursorClipRect();
			}

			break;
			}

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN: {
			switch((int)wParam) {
			case VK_TAB:
			{
			if(hi_bit_mask::state(GetKeyState(VK_SHIFT)))
			w.focusPrevious();
			else
			w.focusNext();

			break;
			}
			}

			send<KeyDownMessage>(w, (int)wParam, (int)(lParam & 0xFFFF), (get_bit<30>(lParam) == 0), (get_bit<24>(lParam) != 0));
			break;
			}

			case WM_CHAR: {
			send<CharMessage>(w, (int)wParam, (int)(lParam & 0xFFFF), (get_bit<30>(lParam) == 0), (get_bit<24>(lParam) != 0));
			break;
			}

			case WM_HOTKEY: {
			send<HotkeyMessage>(w, (int)wParam);
			break;
			}

			case WM_KEYUP:
			case WM_SYSKEYUP: {
			send<KeyUpMessage>(w, (int)wParam, (int)(lParam & 0xFFFF), (get_bit<30>(lParam) == 0), (get_bit<24>(lParam) != 0));
			break;
			}

			case WM_LBUTTONDOWN: {
			send<MouseDownMessage>(w, MouseButton::Left, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
			break;
			}

			case WM_MBUTTONDOWN: {
			send<MouseDownMessage>(w, MouseButton::Middle, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
			break;
			}

			case WM_RBUTTONDOWN: {
			send<MouseDownMessage>(w, MouseButton::Right, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
			break;
			}

			case WM_LBUTTONUP: {
			w._mouseState.unpress(MouseButton::Left);
			send<MouseUpMessage>(w, MouseButton::Left, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
			break;
			}

			case WM_MBUTTONUP: {
			w._mouseState.unpress(MouseButton::Middle);
			send<MouseUpMessage>(w, MouseButton::Middle, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
			break;
			}

			case WM_RBUTTONUP: {
			w._mouseState.unpress(MouseButton::Right);
			send<MouseUpMessage>(w, MouseButton::Right, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
			break;
			}

			case WM_LBUTTONDBLCLK: {
			send<MouseDblClickMessage>(root, MouseButton::Left, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
			break;
			}

			case WM_MBUTTONDBLCLK: {
			send<MouseDblClickMessage>(root, MouseButton::Middle, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
			break;
			}

			case WM_RBUTTONDBLCLK: {
			send<MouseDblClickMessage>(root, MouseButton::Right, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);
			break;
			}

			case WM_MOUSEWHEEL: {
			send<MouseWheelMessage>(root, GET_WHEEL_DELTA_WPARAM(wParam), (int)LOWORD(lParam), (int)HIWORD(lParam), (int)LOWORD(wParam));
			break;
			}

			case WM_SIZING: {
			w.update();
			break;
			}
			*/
			case WM_WINDOWPOSCHANGED: {
				// if (!w._shown) {
				// 	break;
				// }

				WINDOWPOS & pos = *(WINDOWPOS *)lParam;

				RectAdapter a;
				GetClientRect(w.handle(), &a.rect);
				a.assign_to(w._area);
				w._area_changed.get_subscriber().on_next(w._area);

				/*
				if(pos.x != w._outerRegion.left || pos.y != w._outerRegion.top)
				send<UIMoveMessage>(w, a.rect.left, a.rect.top);

				w._outerRegion.set_placement(pos.x, pos.y, pos.cx, pos.cy);

				WINDOWPLACEMENT wp {};
				GetWindowPlacement(hWnd, &wp);

				if(wp.showCmd == SW_SHOWNORMAL || w._width != a.width() || w._height != a.height()) {
				w._width = a.width();
				w._height = a.height();

				send<UIResizeMessage>(w, w._width, w._height);
				}

				if(check_flag(SWP_HIDEWINDOW, pos.flags)) {
				w._state = WindowState::Hidden;
				break;
				}

				switch(wp.showCmd) {
				case SW_HIDE:
				w._state = WindowState::Hidden;
				break;

				case SW_MAXIMIZE:
				w._state = WindowState::Maximized;
				break;

				case SW_MINIMIZE:
				case SW_SHOWMINIMIZED:
				case SW_SHOWMINNOACTIVE:
				w._state = WindowState::Minimized;
				break;

				case SW_RESTORE:
				case SW_SHOWNORMAL:
				w._state = WindowState::Normal;
				break;

				case SW_SHOW:
				break;
				}

				w.update();*/

				break;
			}

			case WM_ERASEBKGND:
				return 1;

			default:
				return DefWindowProcW(hWnd, message, wParam, lParam);
		}

		return 0;
	}

	window_inputs::window_inputs(const rx::observable<math::int_rect> & area) :
		area(area),
		position(area.map([](const auto & r) { return r.pos(); }).distinct_until_changed()),
		size(area.map([](const auto & r) { return r.size(); }).distinct_until_changed())
	{

	}

	window::window(const string & caption, const math::int_rect & area, const char * displayName) :
		inputs(_area_changed.get_observable()),
		_area(area)
	{
		_handle = window::create_handle(area, asd::from_utf8(caption), L"ASDWindowClass", wndProc);
		_device = ::GetDC(_handle);

		/*
		RectAdapter a;
		GetWindowRect(_handle, &a.rect);
		a.assignTo(_outerRegion);

		_normalStyle = GetWindowLongW(_handle, GWL_STYLE);
		*/
		SetWindowLongPtrW(_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		ShowWindow(_handle, SW_HIDE);
	}

	window::~window() {
		if (_device != nullptr) {
			::ReleaseDC(_handle, _device);
			::DeleteDC(_device);
		}

		if (_handle != nullptr) {
			::DestroyWindow(_handle);
		}
	}

	void window::show() {
		ShowWindow(_handle, SW_NORMAL);
	}

	void window::hide() {
		ShowWindow(_handle, SW_HIDE);
	}

	int window::process_events() {
		static MSG msg = { 0 };

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				return 0;
			}
		}

		return 1;
	}
}

//---------------------------------------------------------------------------
