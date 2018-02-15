//---------------------------------------------------------------------------

#include <application/starter.h>
#include <sdlpp/sdlpp.h>
#include <opengl/shader.h>
#include <opengl/mesh.h>

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

        try {
            SDL_DisplayMode dm;
            SDL_GetCurrentDisplayMode(0, &dm);                                                                          // Get display size

            sdl::window window(app, { 768, 480 });                                                                      // Create window
            auto & context = driver.create_context<sdl::graphics>(window);                                              // Create graphics

            auto shader_program_name = "2d/color";
            std::cout << "Create shader program: " << shader_program_name << std::endl;

            opengl::shader_program program(context, opengl::shader_code::get(shader_program_name));

            auto & layout = opengl::vertex_layouts::p2c3::get();

            auto mesh = color_quad(0.5f, [&](auto && vertices, auto && indices) {
                return opengl::mesh_builder(context, layout, vertices).indices(indices).build();
            });

            return loop(window, [&]() {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                program.apply(context);
                opengl::draw_mesh(context, *mesh);

                context.swap();                                                                                         // Update screen
            });
        } catch (const std::exception & e) {
            std::cout << e.what() << std::endl;
            return 1;
        }
    });
}

//---------------------------------------------------------------------------
