//---------------------------------------------------------------------------

#include <application/starter.h>
#include <sdlpp/sdlpp.h>
#include <opengl/shader.h>
#include <opengl/mesh.h>
#include <opengl/uniform.h>

#include <math/matrix.h>
#include <math/quaternion.h>

// #include <vld.h>

//---------------------------------------------------------------------------

namespace asd
{
    template <class F>
    int loop(sdl::window & window, F job) {
        SDL_Event e;

        while (true) {
            while (SDL_PollEvent(&e) != 0) {
                switch (e.type) {
                    case SDL_WINDOWEVENT: {
                        switch (e.window.event) {
                            case SDL_WINDOWEVENT_SIZE_CHANGED:
                                window.set_size({ e.window.data1, e.window.data2 });
                                break;

                            default: {
                            }
                        }

                        break;
                    }

                    case SDL_QUIT:
                        return 0;

                    default: {
                    }
                }
            }

            job();
        }
    }

    template <class F>
    auto cube(F cb) {
        static gfx3d::vertex_data vertices{
            -1.0f, +1.0f, +1.0f, // ltf 0
            -1.0f, -1.0f, +1.0f, // lbf 1
            +1.0f, -1.0f, +1.0f, // rbf 2
            +1.0f, +1.0f, +1.0f, // rtf 3
            -1.0f, +1.0f, -1.0f, // ltb 4
            -1.0f, -1.0f, -1.0f, // lbb 5
            +1.0f, -1.0f, -1.0f, // rbb 6
            +1.0f, +1.0f, -1.0f, // rtb 7
        };

        static gfx3d::vertex_indices indices{
            0, 1, 2, 2, 3, 0, // front
            7, 6, 5, 5, 4, 7, // back
            4, 5, 1, 1, 0, 4, // left
            3, 2, 6, 6, 7, 3, // right
            1, 5, 6, 6, 2, 1, // bottom
            4, 0, 3, 3, 7, 4, // top
        };

        return cb(vertices, indices);
    }

    template <class F>
    auto quad(float scale, F cb) {
        static gfx3d::vertex_data vertices{
            -scale, +scale, -0.0f, // lt 0
            -scale, -scale, -0.0f, // lb 1
            +scale, -scale, -0.0f, // rb 2
            +scale, +scale, -0.0f, // rt 3
        };

        static gfx3d::vertex_indices indices{
            0, 1, 2,
            2, 3, 0
        };

        return cb(vertices, indices);
    }

    template <class F>
    auto color_quad(float scale, F cb) {
        static gfx3d::vertex_data vertices{
            -scale, +scale, 1.0f, 0.5f, 0.5f, // lt 0
            -scale, -scale, 1.0f, 1.0f, 1.0f, // lb 1
            +scale, -scale, 0.5f, 0.5f, 1.0f, // rb 2
            +scale, +scale, 1.0f, 1.0f, 1.0f, // rt 3
        };

        static gfx3d::vertex_indices indices{
            0, 1, 2,
            2, 3, 0
        };

        return cb(vertices, indices);
    }

    static entrance open([]() {
        // Let's start from here!

        sdl::app app;
        opengl::driver driver;

        namespace uniform = gfx3d::uniform;

        SDL_DisplayMode dm;
        SDL_GetCurrentDisplayMode(0, &dm);                                                                          // Get display size

        sdl::window window(app, { 768, 480 });                                                                      // Create window
        auto & context = driver.create_context<sdl::graphics>(window);                                              // Create graphics
            
        auto & uniforms = get<uniform::component>(context);
        auto & color_uniform = uniforms.register_uniform("Color", {uniform::scheme::create<uniform::f32v4>("color")});
        auto & model_uniform = uniforms.register_uniform("Model", { uniform::scheme::create<uniform::f32m4>("model") });
        auto & view_uniform = uniforms.register_uniform("View", { uniform::scheme::create<uniform::f32m4>("view") });
        auto & projection_uniform = uniforms.register_uniform("Projection", { uniform::scheme::create<uniform::f32m4>("projection") });
            
        auto color_block = color_uniform.create_block();
        color_block.set(0, gfx::colorf(1, 0.5f, 0.5f));

        auto line_color_block = color_uniform.create_block();
        line_color_block.set(0, gfx::colorf(1, 0.5f, 0.5f));

        auto scaling = math::fmat::scaling({ 0.5f, 0.5f, 0.5f });
        math::fquat rotation;

        auto get_transform = [&]() {
            return rotation.to_matrix() * scaling;
        };

        auto model_block = model_uniform.create_block();
        model_block.set<true>(0, get_transform());

        auto line_model_block = model_uniform.create_block();
        line_model_block.set<true>(0, get_transform());

        auto view_block = view_uniform.create_block();
        view_block.set<true>(0, math::fmat::look_at({ 2.0f, 2.0f, -2.0f }, { 0, 0, 0.0f }, { 0, 1.0f, 0 }));
            
        auto projection_block = projection_uniform.create_block();
        projection_block.set<true>(0, math::fmat::perspective(60.0f, 480.0f / 768.0f, 0.001f, 100.0f));
            
        auto shader_program_name = "3d/color";
        std::cout << "Create shader program: " << shader_program_name << std::endl;

        opengl::shader_program program(context, opengl::shader_code::get(shader_program_name));

        auto & layout = opengl::vertex_layouts::p3::get();

        auto mesh = cube([&](auto && vertices, auto && indices) {
            return opengl::mesh_builder(context, layout, vertices).indices(indices).build();
        });

        window.size_input().subscribe([&](const math::int_size & size) {
            projection_block.set<true>(0, math::fmat::perspective(60.0f, float(size.y) / float(size.x), 0.001f, 100.0f));
        });

        return loop(window, [&]() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            rotation.rotate_x(0.01f);

            scaling = math::fmat::scaling(0.5f);
            model_block.set<true>(0, get_transform());
            color_block.set(0, gfx::colorf(1.0f, 1.0f, 1.0f));

            scaling = math::fmat::scaling(0.51f);
            line_model_block.set<true>(0, get_transform());
            line_color_block.set(0, gfx::colorf(0.5f, 0.5f, 0.5f));

            model_uniform.update();
            view_uniform.update();
            projection_uniform.update();
            color_uniform.update();

            program.apply();

            model_block.bind();
            color_block.bind();

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            opengl::draw_mesh(context, *mesh);

            line_model_block.bind();
            line_color_block.bind();

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            opengl::draw_mesh(context, *mesh);

            context.swap();                                                                                         // Update screen
        });
    });
}

//---------------------------------------------------------------------------
