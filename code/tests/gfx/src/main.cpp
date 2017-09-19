//---------------------------------------------------------------------------

#include <application/starter.h>
#include <opengl/opengl.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd {
	void draw_quad() {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	}
	
	void processEvents(window & w) {

#if BOOST_OS_WINDOWS

#elif BOOST_OS_LINUX
		XWindowAttributes gwa;
		XEvent xev;
		
		while(true) {
			if(XPending(w.display())) {
				XNextEvent(w.display(), &xev);
				
				switch(xev.type) {
					case Expose: {
						break;
					}
					
					case KeyPress: {
						if(xev.xkey.keycode == 9) {
							std::cout << "Exit..." << std::endl;
							return;
						}
						
						break;
					}
					
					default: {}
				}
			} else {
				XGetWindowAttributes(w.display(), w.handle(), &gwa);
				glViewport(0, 0, gwa.width, gwa.height);
				
				draw_quad();
				
				glXSwapBuffers(w.display(), w.handle());
			}
		}
#endif
	}
	
	static entrance open([]() {
		opengl::driver driver;
		
		try {
			window w(driver);
			
			std::cout << "Press ESC to exit" << std::endl;
			
			processEvents(w);
			
			return 0;
		} catch(const WindowCreationException & e) {
			std::cerr << e.what() << std::endl;
		} catch(const GraphicsException & e) {
			std::cerr << e.what() << std::endl;
		}
		
		return 0;
	});
}

//---------------------------------------------------------------------------
