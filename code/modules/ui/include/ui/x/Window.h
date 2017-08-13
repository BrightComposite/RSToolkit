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
		api(ui) Window(asd::graphics * graphics, const int_rect & rect, const String & caption = L"");
		
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
		
		api(ui) void centralize() {} //;
		
		virtual api(ui) void registerHotkey(int id, int key, int modifiers = 0) override {} //;
		virtual api(ui) void unregisterHotkey(int id) override {} //;
		
		bool isFullscreen() const
		{
			return _fullscreen;
		}
		
		api(ui) void setFullscreen(bool fullscreen) {} //;
		api(ui) void toggleFullscreen() {} //;
		
		api(ui) void setState(WindowState state) {} //;
		
		api(ui) void show();
		api(ui) void hide();
		api(ui) void minimize() {} //;
		api(ui) void maximize() {} //;
		api(ui) void restore() {} //;
		
		BorderStyle borderStyle() const
		{
			return _borderStyle;
		}
		
		api(ui) void setBorderStyle(BorderStyle style) {} //;
		
		bool isVisible() const
		{
			return _state != WindowState::Hidden;
		}
		
		WindowState state() const
		{
			return _state;
		}
		
		api(ui) void setCaption(const String & caption);
		api(ui) String getCaption();
		
		api(ui) void close() {} //;
	
	protected:
		api(ui) void makeFullscreen() {} //;
		api(ui) void restoreSize() {} //;
		api(ui) void applyBorderStyle() {} //;
		
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
