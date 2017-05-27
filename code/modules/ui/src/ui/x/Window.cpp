//---------------------------------------------------------------------------

#include <ui/Window.h>

//---------------------------------------------------------------------------

namespace asd
{
	implement_link(Window);
	
	Window::Window(Graphics * graphics, const IntRect & rect, const String & caption) : UISpace(graphics, rect.size()) {
		setclass(Window);
		
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
