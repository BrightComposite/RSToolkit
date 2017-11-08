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
	
	int launch(window & w, opengl::context & context);
	
	static entrance open([]() {
		opengl::driver driver;
		
		try {
			window w("gfx:test", {50, 50, 400, 400});
			auto & context = w.bind(driver);
			
			std::cout << "Press ESC to exit" << std::endl;
			
			
			w.show();
			
			launch(w, context);
		} catch(const WindowCreationException & e) {
			std::cerr << "Window creation error: " << e.what() << std::endl;
		} catch(const GraphicsException & e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
	});
	
	int launch(window & w, opengl::context & context) {
		benchmark raw("draw triangle opengl");
		benchmark queued("draw triangle queued");
		benchmark immediate("draw triangle immediately");

		auto & layout = opengl::vertex_layouts::p2c3::get();

		gfx3d::vertex_data data({
			-0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 1.0f,
		});

		auto mesh = opengl::mesh_builder::build(context, layout, data);
		opengl::shader_program program(context, opengl::shader_code::get(COLOR_SHADER::key()));

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
					
					default: {
					}
				}
				
				continue;
			}

#endif
			
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			double r = raw([&]() {
				program.apply(context);
				opengl::draw_mesh(context, *mesh);
				
				context.flush();
			});
			
			std::cout << r / 1000'000 << "ms" << " " << std::endl;
		}
	}
}

//---------------------------------------------------------------------------
