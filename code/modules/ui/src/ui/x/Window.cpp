//---------------------------------------------------------------------------

#include <ui/Window.h>

//---------------------------------------------------------------------------

namespace asd
{
	implement_link(Window);
	
	Window::Window(Graphics * graphics, const IntRect & rect, const WideString & caption) : UISpace(graphics, rect.size()) {
		setclass(Window);
		
		auto d = XOpenDisplay(NULL);
		
		if(d == nullptr) {
			throw Exception("Can't open X display");
		}
		
		auto s = DefaultScreen(d);
		
		auto depth = DefaultDepth(d, s);
		auto visual = DefaultVisual(d, s);
		
		auto root = DefaultRootWindow(d);
		XSetWindowAttributes swa;
		swa.colormap = XCreateColormap(d, root, visual, AllocNone);
		swa.event_mask = ExposureMask | KeyPressMask;
		
		auto w = XCreateWindow(d, root, 0, 0, 600, 600, 0, depth, InputOutput, visual, CWColormap | CWEventMask, &swa);
		
		XMapWindow(d, w);
		XStoreName(d, w, to_utf8(caption).c_str());
	}
}

//---------------------------------------------------------------------------
