//---------------------------------------------------------------------------

#include <application/starter.h>
#include <opengl/opengl.h>
#include <opengl/mesh.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd 
{
	int processEvents(window & w, gfx::context & context, const opengl::mesh & mesh);
	
	static entrance open([]() {
		opengl::driver driver;
		
		try {
			window w("gfx:test", {20, 20, 180, 180});
			auto & context = w.bind(driver);
			
			std::cout << "Press ESC to exit" << std::endl;

			auto & layout = opengl::vertex_layouts::get("p2");
			
			auto vertex_data = {-0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f};
			opengl::mesh_builder builder(context, layout, vertex_data);
			auto mesh = builder.build();

			std::cout << layout.units << std::endl;

			w.show();

			processEvents(w, context, mesh);
		} catch(const WindowCreationException & e) {
			std::cerr << e.what() << std::endl;
		} catch(const std::out_of_range & e) {
			std::cerr << e.what() << std::endl;
		} catch(const GraphicsException & e) {
			std::cerr << e.what() << std::endl;
		}
	});

	int processEvents(window & w, gfx::context & context, const opengl::mesh & mesh) {

	#if BOOST_OS_WINDOWS

		while(true) {
			static MSG msg = {0};

			if(PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessageW(&msg);

				if(msg.message == WM_QUIT) {
					return 0;
				}
			} else {
				glClearColor(0.0, 0.0, 0.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				context << mesh;

				context.draw();

				SwapBuffers(w.device());
			}
		}

	#elif BOOST_OS_LINIX

		XWindowAttributes gwa;
		XEvent xev;

		while(true) {
			if(XPending(w.display())) {
				XNextEvent(w.display(), &xev);

				switch(xev.type) {
					case Expose: {
						XGetWindowAttributes(w.display(), w.handle(), &gwa);
						glViewport(0, 0, gwa.width, gwa.height);

						break;
					}

					case KeyPress: {
						if(xev.xkey.keycode == 9) {
							std::cout << "Exit..." << std::endl;
							return 0;
						}

						break;
					}

					default: {}
				}
			} else {
				draw_quad();

				glXSwapBuffers(w.display(), w.handle());
			}
		}

	#endif
	}

}

//---------------------------------------------------------------------------
