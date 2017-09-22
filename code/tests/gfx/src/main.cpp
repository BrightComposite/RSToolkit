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
		using tag = opengl::shader_program;

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

		driver.register_method(draw_colored_mesh);

		driver.register_shader<COLOR_SHADER>();

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

			auto mesh = opengl::mesh_builder::build(context, layout, data);
			
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

		benchmark raw("draw triangle opengl");
		benchmark queued("draw triangle queued");
		benchmark immediate("draw triangle immediately");

		while(true) {
#if BOOST_OS_WINDOWS

			static MSG msg = {0};

			if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if(msg.message == WM_QUIT) {
					return 0;
				}

				continue;
			}

#elif BOOST_OS_LINUX

			static XWindowAttributes gwa;
			static XEvent xev;

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

				continue;
			}

#endif
			
			auto & glmesh = static_cast<const opengl::generic_mesh<opengl::mesh_type::plain> &>(e.mesh);
			auto & glprogram = static_cast<const opengl::shader_program &>(e.shader_program);

			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			double r = raw([&]() {
				glUseProgram(glprogram.id);

				glBindVertexArray(glmesh.id);

				for(int i = 0; i < 10'000; ++i) {
					glDrawArrays(glmesh.topology, glmesh.offset, glmesh.verticesCount);
				}

				glBindVertexArray(0);

				context.flush();
			});

			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			double q = queued([&]() {
				for(int i = 0; i < 10'000; ++i) {
					context << e;
				}

				context.flush();
			});

			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			double i = immediate([&]() {
				for(int i = 0; i < 10'000; ++i) {
					draw_colored_mesh(context, e);
				}

				context.flush();
			});

			std::cout << r / 1000'000 << "ms" << " " << i / 1000'000 << "ms" << " " << q / 1000'000 << "ms" << std::endl;
		}
	}
}

//---------------------------------------------------------------------------
