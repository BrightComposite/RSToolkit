//---------------------------------------------------------------------------

#pragma once

#ifndef X_WINDOW_H
#define X_WINDOW_H

#ifndef WINDOW_H
	#error "You shouldn't include this file directly! Include <ui/Window.h> instead"
#endif

//---------------------------------------------------------------------------

#include <ui/WindowEnum.h>
#include <ui/ui_space.h>

//---------------------------------------------------------------------------

namespace asd
{
	class Window;
	
	class Window : public ui_space
	{
	public:
		Window(asd::graphics * graphics, long left, long top, long width, long height, const String & caption = L"") : Window(graphics, int_rect {left, top, left + width, top + height}, caption) {}
		Window(asd::graphics * graphics, long width, long height, const String & caption = L"") : Window(graphics, int_size {width, height}, caption) {}
		Window(asd::graphics * graphics, const int_size & size, const String & caption = L"") : Window(graphics, int_rect {0, 0, size.x, size.y}, caption) {}
		api(gui) Window(asd::graphics * graphics, const int_rect & rect, const String & caption = L"");
		
		virtual ~Window() {}
		
		long outerWidth() const
		{
			return _outerRegion.width();
		}
		
		long outerHeight() const
		{
			return _outerRegion.height();
		}
		
		int_size outerSize() const
		{
			return _outerRegion.size();
		}
		
		int_rect outerRegion() const
		{
			return _outerRegion;
		}
		
		api(gui) void centralize() {} //;
		
		virtual api(gui) void registerHotkey(int id, int key, int modifiers = 0) override {} //;
		virtual api(gui) void unregisterHotkey(int id) override {} //;
		
		bool isFullscreen() const
		{
			return _fullscreen;
		}
		
		api(gui) void setFullscreen(bool fullscreen) {} //;
		api(gui) void toggleFullscreen() {} //;
		
		api(gui) void setState(WindowState state) {} //;
		
		api(gui) void show();
		api(gui) void hide();
		api(gui) void minimize() {} //;
		api(gui) void maximize() {} //;
		api(gui) void restore() {} //;
		
		BorderStyle borderStyle() const
		{
			return _borderStyle;
		}
		
		api(gui) void setBorderStyle(BorderStyle style) {} //;
		
		bool isVisible() const
		{
			return _state != WindowState::Hidden;
		}
		
		WindowState state() const
		{
			return _state;
		}
		
		api(gui) void setCaption(const String & caption);
		api(gui) String getCaption();
		
		api(gui) void close() {} //;
	
	protected:
		api(gui) void makeFullscreen() {} //;
		api(gui) void restoreSize() {} //;
		api(gui) void applyBorderStyle() {} //;
		
		bool _shown = false;
		
		int_rect _outerRegion;
		
		WindowState _state = WindowState::Hidden;
		BorderStyle _borderStyle = BorderStyle::Normal;
		
		bind_messages(Window, WindowMessages);
	};
	
	channels_api(ui, Window, WindowMessages)
}

//---------------------------------------------------------------------------
#endif
