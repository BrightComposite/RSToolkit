//---------------------------------------------------------------------------

#include <ui/Window.h>

//---------------------------------------------------------------------------

namespace asd
{
	implement_link(Window);
	
	Window::Window(Graphics * graphics, const IntRect & rect, const WideString & caption) : UISpace(graphics, rect.size()) {
		setclass(Window);
		
		setCaption(caption);
	}
	
	void Window::show() {
		XMapWindow(_display, _handle);
	}
	
	void Window::hide() {
		XUnmapWindow(_display, _handle);
	}
	
	void Window::setCaption(const WideString & caption) {
		XStoreName(_display, _handle, to_utf8(caption).c_str());
	}
	
	WideString Window::getCaption() {
		char * caption = nullptr;
		XFetchName(_display, _handle, &caption);
		WideString out = from_utf8(caption);
		XFree(caption);
		return out;
	}
}

//---------------------------------------------------------------------------
