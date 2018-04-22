//---------------------------------------------------------------------------

#include <ui/Window.h>

//---------------------------------------------------------------------------

namespace asd
{
	Window::Window(asd::graphics * graphics, const int_rect & rect, const String & caption) : ui_space(graphics, rect.size()) {
		setCaption(caption);
	}
	
	void Window::show() {
		XMapWindow(_display, _handle);
	}
	
	void Window::hide() {
		XUnmapWindow(_display, _handle);
	}
	
	void Window::setCaption(const String & caption) {
		XStoreName(_display, _handle, caption.c_str());
	}
	
	String Window::getCaption() {
		char * caption = nullptr;
		XFetchName(_display, _handle, &caption);
		String out(caption);
		XFree(caption);
		return out;
	}
}

//---------------------------------------------------------------------------
