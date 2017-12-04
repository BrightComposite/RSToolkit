//---------------------------------------------------------------------------

#include <window/window.h>

//---------------------------------------------------------------------------

namespace asd
{
	window::window(const string & caption, const math::int_rect & area, const char * displayName) : _area(area) {
		_display = XOpenDisplay(displayName);
		
		if (_display == nullptr) {
			throw open_display_exception(displayName);
		}
		
		this->area.subscribe([this](const math::int_rect & rect) {
			auto p = rect.pos();
			auto s = rect.size();
			
			if (position.get() != p) {
				position.set(p);
			}
			
			if (size.get() != s) {
				size.set(s);
			}
		});
	}
	
	window::~window() {
		_context.reset();										// clear context first so it can free own resources correctly
		
		if (_display != nullptr) {								// if display was created successfully earlier
			if (_handle != 0) {									// if window handle was created successfully earlier
				XDestroyWindow(_display, _handle);
				_handle = 0;									// clear handle
			}
			
			XCloseDisplay(_display);
			_display = nullptr;									// clear display pointer
		}
	}
	
	void window::init(XVisualInfo * visual_info) {
		auto screen = DefaultScreen(_display);
		auto root = RootWindow(_display, screen);
		
		{
			XSetWindowAttributes winAttr;
			
			winAttr.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask;
			winAttr.background_pixmap = 0;
			winAttr.background_pixel = 0;
			winAttr.border_pixel = 0;
			
			winAttr.colormap = XCreateColormap(_display, root, visual_info->visual, AllocNone);
			
			unsigned int mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;
			
			_handle = XCreateWindow(_display, root, _area.left, _area.top, _area.width(), _area.height(), 0, visual_info->depth, InputOutput, visual_info->visual, mask, &winAttr);
		}
		
		if (!_handle) {
			throw window_creation_exception("Cannot create X Window!");
		}
		
		XStoreName(_display, _handle, "ASD");
	}
	
	void window::show() {
		XMapWindow(_display, _handle);
	}
	
	void window::hide() {
		XUnmapWindow(_display, _handle);
	}
	
	int window::process_events() {
		if (XPending(_display)) {
			XNextEvent(_display, &_event);
			
			switch (_event.type) {
				case Expose: {
					XGetWindowAttributes(_display, _handle, &_attributes);
					_area.set_placement(_attributes.x, _attributes.y, _attributes.width, _attributes.height);
					
					if (area.get() != _area) {
						area.set(_area);
					}
					
					break;
				}
				
				case KeyPress: {
					if (_event.xkey.keycode == 9) {
						return 0;
					}
					
					break;
				}
				
				default: {
				}
			}
		}
		
		return 1;
	}
}

//---------------------------------------------------------------------------
