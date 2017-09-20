//---------------------------------------------------------------------------

#include <application/starter.h>
#include <opengl/opengl.h>
#include <opengl/mesh.h>
#include <opengl/shader.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd 
{
	struct colored_mesh : gfx::primitive
	{
		morph_type(colored_mesh);

		colored_mesh(const opengl::mesh & mesh) : mesh(mesh) {}
		colored_mesh(const colored_mesh & e) : mesh(e.mesh) {}

		opengl::mesh mesh;
	};

	int processEvents(window & w, gfx::context & context, const colored_mesh & e);

	static entrance open([]() {
		opengl::driver driver;

		driver << [](opengl::context & ctx, const colored_mesh & e) {
			// change to ctx.shader_program(...)
			static opengl::shader_program program(ctx, opengl::shader_code::get("2d/color"));

			opengl::apply(ctx, program);
			opengl::draw_mesh(ctx, e.mesh);
		};

		try {
			window w("gfx:test", {20, 20, 180, 180});
			auto & context = w.bind(driver);
			
			std::cout << "Press ESC to exit" << std::endl;

			auto & layout = opengl::vertex_layouts::p2::get();
			
			opengl::mesh_builder builder(context, layout, {-0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f});
			auto mesh = builder.build();

			colored_mesh e {mesh};

			w.show();

			processEvents(w, context, e);
		} catch(const WindowCreationException & e) {
			std::cerr << "Window creation error: " << e.what() << std::endl;
		} catch(const GraphicsException & e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
	});

	int processEvents(window & w, gfx::context & context, const colored_mesh & e) {

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

				context << e;
				context.draw();

				SwapBuffers(w.device());
			}
		}

#elif BOOST_OS_LINUX

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
				context << mesh;
				
				context.draw();
				
				glXSwapBuffers(w.display(), w.handle());
			}
		}

#endif
	}

}

//---------------------------------------------------------------------------
