//---------------------------------------------------------------------------

#pragma once

#ifndef WINDOW_H
#define WINDOW_H

//---------------------------------------------------------------------------

#include "UISpace.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Window;

	enum class WindowState
	{
		Normal,
		Hidden,
		Minimized,
		Maximized
	};

	enum class BorderStyle
	{
		Normal,
		Static,
		Fixed,
		None
	};

	link_class(ui, Window, Class<UISpace>);

	class Window : public UISpace
	{
	public:
		Window(Graphics * graphics, long left, long top, long width, long height, const WideString & caption = L"") : Window(graphics, IntRect {left, top, left + width, top + height}, caption) {}
		Window(Graphics * graphics, long width, long height, const WideString & caption = L"") : Window(graphics, IntSize {width, height}, caption) {}
		Window(Graphics * graphics, const IntSize & size, const WideString & caption = L"") : Window(graphics, IntRect {0, 0, size.x, size.y}, caption) {}
		api(ui) Window(Graphics * graphics, const IntRect & rect, const WideString & caption = L"");

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

		api(ui) void centralize();

		virtual api(ui) void registerHotkey(int id, int key, int modifiers = 0) override;
		virtual api(ui) void unregisterHotkey(int id) override;

		bool isFullscreen() const
		{
			return _fullscreen;
		}

		api(ui) void setFullscreen(bool fullscreen);
		api(ui) void toggleFullscreen();

		api(ui) void setState(WindowState state);

		api(ui) void show();
		api(ui) void hide();
		api(ui) void minimize();
		api(ui) void maximize();
		api(ui) void restore();

		BorderStyle borderStyle() const
		{
			return _borderStyle;
		}

		api(ui) void setBorderStyle(BorderStyle style);

		bool isVisible() const
		{
			return _state != WindowState::Hidden;
		}

		WindowState state() const
		{
			return _state;
		}

		api(ui) void setCaption(const WideString & caption);
		api(ui) WideString getCaption();

		api(ui) void close();
		api(ui) void attachThread();

	protected:
		api(ui) void makeFullscreen();
		api(ui) void restoreSize();
		api(ui) void applyBorderStyle();

		friend api(ui) LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool _isShown = false;
		bool _isActive = false;

		DWORD _normalStyle;
		LongRect _outerRegion;

		WINDOWPLACEMENT _fullscreenPlacement = {0};
		WINDOWPLACEMENT _windowedPlacement = {0};

		WindowState _state = WindowState::Hidden;
		BorderStyle _borderStyle = BorderStyle::Normal;

		bind_messages(Window, WindowMessages);
	};

	channels_api(ui, Window, WindowMessages)

	inline bool isKeyPressed(int v_key)
	{
		return hi_bit_mask::state(GetKeyState(v_key));
	}
}

//---------------------------------------------------------------------------
#endif
