//---------------------------------------------------------------------------

#pragma once

#ifndef COMPONENTS_H
#define COMPONENTS_H

//---------------------------------------------------------------------------

#include <SDL.h>
#include <math/rect.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	namespace sdl
	{
		struct exception {};
		
		struct app
		{
			app() {
				if (SDL_Init(SDL_INIT_VIDEO) < 0) {
					std::cout << "Couldn't initialize SDL!" << std::endl;
					throw exception();
				}
			}
			
			~app() {
				SDL_Quit();
			}
		};
		
		struct window
		{
			window(const sdl::app &, const asd::math::int_size & size) {
				handle = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
				
				if (handle == nullptr) {
					std::cout << "Couldn't create window!" << std::endl;
					throw exception();
				}
			}
			
			~window() {
				if (handle != nullptr) {
					SDL_DestroyWindow(handle);
				}
			}
            
            operator SDL_Window * () {
                return handle;
            }
            
            SDL_Window * operator -> () {
                return handle;
            }
			
			SDL_Window * handle = nullptr;
		};
		
		class surface
		{
        public:
			surface(sdl::window & window) {
				handle = SDL_GetWindowSurface(window);
				
				if (handle == nullptr) {
					std::cout << "Couldn't create window surface!" << std::endl;
					throw exception();
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
            
            bool operator == (SDL_Surface * h) const {
                return handle == h;
            }
            
            bool operator != (SDL_Surface * h) const {
                return handle != h;
            }
            
            operator SDL_Surface * () {
                return handle;
            }
            
            SDL_Surface * operator -> () {
                return handle;
            }
			
        private:
			SDL_Surface * handle = nullptr;
		};
        
        class renderer
        {
        public:
            renderer(sdl::window & window) {
                handle = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
                
                if (handle == nullptr) {
                    std::cout << "Can't create renderer" << std::endl;
                    throw exception();
                }
            }
            
            ~renderer() {
                if (!handle) {
                    SDL_DestroyRenderer(handle);
                }
            }
            
            operator SDL_Renderer * () {
                return handle;
            }
    
            SDL_Renderer * operator -> () {
                return handle;
            }
            
        private:
            SDL_Renderer * handle = nullptr;
        };
		
		class texture
		{
        public:
			texture(sdl::renderer & renderer, const std::string & path) {
                sdl::surface surface(SDL_LoadBMP(path.data()));
                
				if (surface == nullptr) {
					std::cout << "Unable to load image " << path << "!" << std::endl;
					throw exception();
				}
                
                handle = SDL_CreateTextureFromSurface(renderer, surface);
				
				if (handle == nullptr) {
					std::cout << "Unable to create texture " << path.c_str() << std::endl;
					throw exception();
				}
			}
            
            ~texture() {
                SDL_DestroyTexture(handle);
            }
            
            operator SDL_Texture * () {
                return handle;
            }
            
            SDL_Texture * operator -> () {
                return handle;
            }
			
        private:
			SDL_Texture * handle;
		};
	}
}

//---------------------------------------------------------------------------
#endif
