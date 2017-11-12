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
		} catch (const WindowCreationException & e) {
			std::cerr << "Window creation error: " << e.what() << std::endl;
		} catch (const GraphicsException & e) {
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
			 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
		});
		
		auto mesh = opengl::mesh_builder::build(context, layout, data);
		opengl::shader_program program(context, opengl::shader_code::get(COLOR_SHADER::key()));
		
		return w.loop([&]() {
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			double r = raw([&]() {
				program.apply(context);
				opengl::draw_mesh(context, *mesh);
				
				context.flush();
			});
			
			std::cout << r / 1000'000 << "ms" << " " << std::endl;
		});
	}
}

//---------------------------------------------------------------------------
