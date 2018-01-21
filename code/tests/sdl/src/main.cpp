//---------------------------------------------------------------------------

#include <application/starter.h>
#include <SDL.h>
#include <math/rect.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
    namespace sdl
    {
        struct sdl_exception {};
        
        struct app
        {
            app() {
                if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                    std::cout << "Couldn't initialize SDL!" << std::endl;
                    throw sdl_exception();
                }
            }
            
            ~app() {
                SDL_Quit();
            }
        };
        
        struct window
        {
            window(const app &, const math::int_size & size) {
                handle = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                
                if (handle == nullptr) {
                    std::cout << "Couldn't create window!" << std::endl;
                    throw sdl_exception();
                }
            }
            
            ~window() {
                if (handle != nullptr) {
                    SDL_DestroyWindow(handle);
                }
            }
            
            SDL_Window * handle = nullptr;
        };
        
        struct surface
        {
            surface(window & w) {
                handle = SDL_GetWindowSurface(w.handle);
                
                if (handle == nullptr) {
                    std::cout << "Couldn't create window surface!" << std::endl;
                    throw sdl_exception();
                }
            }
            
            surface(SDL_Surface * handle) : handle(handle) {}
            
            surface(surface && s) : handle(s.handle) {
                s.handle = nullptr;
            }
            
            surface & operator =(surface && s) {
                std::swap(handle, s.handle);
                return *this;
            }
            
            ~surface() {
                if (handle != nullptr) {
                    SDL_FreeSurface(handle);
                }
            }
            
            SDL_Surface * handle = nullptr;
        };
        
        struct bitmap
        {
            bitmap(const surface & window_surface, const std::string & path) : surface(SDL_LoadBMP(path.data())) {
                if (surface.handle == nullptr) {
                    std::cout << "Unable to load image " << path << "!" << std::endl;
                    throw sdl_exception();
                }
                
                auto converted = SDL_ConvertSurface(surface.handle, window_surface.handle->format, 0);
                
                if (converted == nullptr) {
                    std::cout << "Unable to optimize image!" << path.c_str() << std::endl;
                    throw sdl_exception();
                }
                
                surface = converted;
            }
            
            sdl::surface surface;
        };
    }
    
    template <class F>
    int loop(F job) {
        SDL_Event e;
        
        while (true) {
            while (SDL_PollEvent(&e) != 0) {
                switch (e.type) {
                    case SDL_QUIT:
                        return 0;
                    
                    default: {}
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
            sdl::surface window_surface(window);                                                                        // Create window surface
            
            sdl::bitmap hello_world(window_surface, "../../../../assets/hello_world.bmp");                              // Create bitmap
            
            return loop([&]() {
                SDL_Rect stretchRect;
                stretchRect.x = 0;
                stretchRect.y = 0;
                SDL_GetWindowSize(window.handle, &stretchRect.w, &stretchRect.h);
                
                SDL_BlitScaled(hello_world.surface.handle, NULL, window_surface.handle, &stretchRect);                  // Draw bitmap
                SDL_UpdateWindowSurface(window.handle);                                                                 // Update window surface
            });
        } catch (const sdl::sdl_exception & e) {
            std::cout << "SDL error: " << SDL_GetError() << std::endl;
            return 1;
        }
    });
}

//---------------------------------------------------------------------------
