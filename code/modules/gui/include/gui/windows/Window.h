//---------------------------------------------------------------------------

#pragma once

#ifndef W_WINDOW_H
#define W_WINDOW_H

//---------------------------------------------------------------------------

#ifndef WINDOW_H
	#error "You shouldn't include this file directly! Include <ui/Window.h> instead"
#endif

//---------------------------------------------------------------------------

#include <ui/WindowEnum.h>
#include <ui/ui_space.h>

//---------------------------------------------------------------------------

namespace asd
{
	link_class(gui, Window, Class<UISpace>);

	class Window : public UISpace
	{
	public:
		Window(Graphics * graphics, long left, long top, long width, long height, const WideString & caption = L"") : Window(graphics, IntRect {left, top, left + width, top + height}, caption) {}
		Window(Graphics * graphics, long width, long height, const WideString & caption = L"") : Window(graphics, IntSize {width, height}, caption) {}
		Window(Graphics * graphics, const IntSize & size, const WideString & caption = L"") : Window(graphics, IntRect {0, 0, size.x, size.y}, caption) {}
		api(gui) Window(Graphics * graphics, const IntRect & rect, const WideString & caption = L"");

		virtual ~Window() {}

		long outerWidth() const
		{
			return _outerRegion.width();
		}

		long outerHeight() const
		{
			return _outerRegion.height();
		}

		LongSize outerSize() const
		{
			return _outerRegion.size();
		}

		LongRect outerRegion() const
		{
			return _outerRegion;
		}

		api(gui) void centralize();

		virtual api(gui) void registerHotkey(int id, int key, int modifiers = 0) override;
		virtual api(gui) void unregisterHotkey(int id) override;

		bool isFullscreen() const
		{
			return _fullscreen;
		}

		api(gui) void setFullscreen(bool fullscreen);
		api(gui) void toggleFullscreen();

		api(gui) void setState(WindowState state);

		api(gui) void show();
		api(gui) void hide();
		api(gui) void minimize();
		api(gui) void maximize();
		api(gui) void restore();

		BorderStyle borderStyle() const
		{
			return _borderStyle;
		}

		api(gui) void setBorderStyle(BorderStyle style);

		bool isVisible() const
		{
			return _state != WindowState::Hidden;
		}

		WindowState state() const
		{
			return _state;
		}

		api(gui) void setCaption(const WideString & caption);
		api(gui) WideString getCaption();

		api(gui) void close();

	protected:
		api(gui) void makeFullscreen();
		api(gui) void restoreSize();
		api(gui) void applyBorderStyle();

		friend api(gui) LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool _shown = false;

		DWORD _normalStyle;
		LongRect _outerRegion;

		WINDOWPLACEMENT _fullscreenPlacement = {0};
		WINDOWPLACEMENT _windowedPlacement = {0};

		WindowState _state = WindowState::Hidden;
		BorderStyle _borderStyle = BorderStyle::Normal;

		bind_messages(Window, WindowMessages);
	};

	channels_api(gui, Window, WindowMessages)

	inline bool isKeyPressed(int v_key)
	{
		return hi_bit_mask::state(GetKeyState(v_key));
	}
}

//---------------------------------------------------------------------------
#endif
