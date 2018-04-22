//---------------------------------------------------------------------------

#include <application/starter.h>
#include <opengl_window/context.h>
#include <opengl/mesh.h>
#include <opengl/shader.h>
#include <opengl/uniform.h>
#include <math/vector.h>
#include <math/quaternion.h>
#include <iostream>
#include <valarray>

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
        } catch (const window_creation_exception & e) {
            std::cerr << "Window creation error: " << e.what() << std::endl;
        } catch (const graphics_exception & e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });
    
    int launch(window & w, opengl::context & context) {
        benchmark raw("draw triangle opengl");
        benchmark queued("draw triangle queued");
        benchmark immediate("draw triangle immediately");

        namespace uniform = gfx3d::uniform;

        auto & uniforms = get<uniform::component>(context);
        auto & color_uniform = uniforms.register_uniform("Color", { uniform::scheme::create<uniform::f32v4>("color") });
        
        auto color_block = color_uniform.create_block();
        color_block.set(0, gfx::colorf(1.0f, 0.5f, 0.5f, 1.0f));

        auto & layout = opengl::vertex_layouts::p2::get();
        
        gfx3d::vertex_data data({
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.0f,  0.5f,
        });

        auto mesh = opengl::mesh_builder::build(context, layout, data);
        opengl::shader_program program(context, opengl::shader_code::get(COLOR_SHADER::key()));
        
        return w.context([&]() {
            glClearColor(1.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            color_block.set(0, gfx::colorf(1.0f, 0.5f, 0.5f, 1.0f));
            color_uniform.update();

            double r = raw([&]() {
                color_block.bind();

                program.apply();
                opengl::draw_mesh(context, *mesh);
            });

            context.flush();
            
            std::cout << r / 1'000'000 << "ms" << " " << std::endl;
        });
    }
}

//---------------------------------------------------------------------------
