//---------------------------------------------------------------------------

#include <application/starter.h>
#include <opengl/opengl.h>
#include <opengl/mesh.h>
#include <opengl/shader.h>
#include <iostream>

#include <benchmark>

//---------------------------------------------------------------------------

namespace asd 
{
	struct COLOR_SHADER
	{
		static auto key() { return "2d/color"; }
	};

	struct colored_mesh : gfx::primitive
	{
		colored_mesh(const opengl::context & ctx, const opengl::mesh & mesh) : mesh(mesh), shader_program(*ctx.get<COLOR_SHADER>()) {}
		colored_mesh(const colored_mesh & e) : mesh(e.mesh), shader_program(e.shader_program) {}
		
		const opengl::mesh & mesh;
		const opengl::modifier & shader_program;
	};

	void draw_colored_mesh(opengl::context & ctx, const colored_mesh & e) {
		e.shader_program.apply(ctx);
		opengl::draw_mesh(ctx, e.mesh);
	}

	int launch(window & w, opengl::context & context, const colored_mesh & e);

	static entrance open([]() {
		opengl::driver driver;

		driver.method(draw_colored_mesh);

		driver.shader<COLOR_SHADER>();

		try {
			window w("gfx:test", {50, 50, 400, 400});
			auto & context = w.bind(driver);
			
			std::cout << "Press ESC to exit" << std::endl;

			auto & layout = opengl::vertex_layouts::p2c3::get();
			
			vertex_data data({
				-0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				 0.0f,  0.5f, 0.0f, 0.0f, 1.0f,
			});

			opengl::mesh_builder builder(context, layout, data);
			auto mesh = builder.build();
			
			colored_mesh e {context, *mesh};

			w.show();

			launch(w, context, e);
		} catch(const WindowCreationException & e) {
			std::cerr << "Window creation error: " << e.what() << std::endl;
		} catch(const GraphicsException & e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
	});

	int launch(window & w, opengl::context & context, const colored_mesh & e) {
		
		benchmark queued("draw triangle queued");
		benchmark immediate("draw triangle immediately");

#if BOOST_OS_WINDOWS

		while(true) {
			static MSG msg = {0};

			if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if(msg.message == WM_QUIT) {
					return 0;
				}
			} else {
				glClearColor(0.0, 0.0, 0.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				double q = queued([&]() {
					context << e;
					context << e;
					context << e;
					context << e;
					context << e;

					context.draw();

					SwapBuffers(w.device());
				});

				glClearColor(0.0, 0.0, 0.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				double i = immediate([&]() {
					draw_colored_mesh(context, e);
					draw_colored_mesh(context, e);
					draw_colored_mesh(context, e);
					draw_colored_mesh(context, e);
					draw_colored_mesh(context, e);

					SwapBuffers(w.device());
				});

				std::cout << i / q << std::endl;
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
				glClearColor(0.0, 0.0, 0.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
				double q = queued([&]() {
					context << e;
					context << e;
					context << e;
					context << e;
					context << e;
					
					context.draw();
					
					glXSwapBuffers(w.display(), w.handle());
				});
				
				glClearColor(0.0, 0.0, 0.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
				double i = immediate([&]() {
					draw_colored_mesh(context, e);
					draw_colored_mesh(context, e);
					draw_colored_mesh(context, e);
					draw_colored_mesh(context, e);
					draw_colored_mesh(context, e);
					
					glXSwapBuffers(w.display(), w.handle());
				});
				
				std::cout << i / q << " " << static_cast<int>(i) << "/" << static_cast<int>(q) << std::endl;
			}
		}

#endif
	}

}

//---------------------------------------------------------------------------
