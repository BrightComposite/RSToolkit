//---------------------------------------------------------------------------

#include <application/Starter.h>

//---------------------------------------------------------------------------

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	void DrawAQuad() {
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1., 1., -1., 1., 1., 20.);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
		
		glBegin(GL_QUADS);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-.75f, -.75f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(.75f, -.75f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(.75f, .75f, 0.f);
		glColor3f(1.f, 1.f, 0.f);
		glVertex3f(-.75f, .75f, 0.f);
		glEnd();
	}
	
	static Entrance _([]() {
		using namespace std;
		
		auto d = XOpenDisplay(NULL);
		
		if(d == nullptr) {
			cout << "Cannot connect to X server" << endl;
			return 1;
		}
		
		GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
		auto vi = glXChooseVisual(d, 0, att);
		
		if(vi == NULL) {
			cout << "No appropriate visual found" << endl;
			return 1;
		}
		
		auto root = DefaultRootWindow(d);
		XSetWindowAttributes swa;
		swa.colormap = XCreateColormap(d, root, vi->visual, AllocNone);
		swa.event_mask = ExposureMask | KeyPressMask;
		
		auto w = XCreateWindow(d, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
		
		XMapWindow(d, w);
		XStoreName(d, w, "ASD: glxtest");
		
		auto glc = glXCreateContext(d, vi, NULL, GL_TRUE);
		glXMakeCurrent(d, w, glc);
		
		glEnable(GL_DEPTH_TEST);
		
		XWindowAttributes gwa;
		XEvent xev;
		
		cout << "Press ESC to exit" << endl;
		
		while(1) {
			XNextEvent(d, &xev);
			
			if(xev.type == Expose) {
				XGetWindowAttributes(d, w, &gwa);
				glViewport(0, 0, gwa.width, gwa.height);
				DrawAQuad();
				glXSwapBuffers(d, w);
			} else if(xev.type == KeyPress) {
				if(xev.xkey.keycode == 9) {
					cout << "Exit..." << endl;
					
					glXMakeCurrent(d, None, NULL);
					glXDestroyContext(d, glc);
					XDestroyWindow(d, w);
					XCloseDisplay(d);
					break;
				}
			}
		}
		
		return 0;
	});
}

//---------------------------------------------------------------------------
