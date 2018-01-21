//---------------------------------------------------------------------------

#include <application/starter.h>
#include <sdl/components.h>

//---------------------------------------------------------------------------

namespace asd
{
    template <class F>
    int loop(F job) {
        SDL_Event e;
        
        while (true) {
            while (SDL_PollEvent(&e) != 0) {
                switch (e.type) {
                    case SDL_QUIT:
                        return 0;
                    
                    default: {
                    }
                }
            }
            
            job();
        }
    }
    
    static entrance open([]() {
        // Let's start from here!
        
        sdl::app app;
        
        try {
            SDL_DisplayMode dm;
            SDL_GetCurrentDisplayMode(0, &dm);                                                                          // Get display size
            
            sdl::window window(app, { 768, 480 });                                                                      // Create window
            sdl::renderer renderer(window);                                                                             // Create renderer
            
            sdl::texture hello_world(renderer, "../../../../assets/hello_world.bmp");                                   // Create texture
            
            return loop([&]() {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);                                               // Set clear color
                SDL_RenderClear(renderer);                                                                              // Clear screen
                SDL_RenderCopy(renderer, hello_world, NULL, NULL);                                                      // Render texture
                
                SDL_RenderPresent(renderer);                                                                            // Update screen
            });
        } catch (const sdl::exception & e) {
            std::cout << "SDL error: " << SDL_GetError() << std::endl;
            return 1;
        }
    });
}

//---------------------------------------------------------------------------
