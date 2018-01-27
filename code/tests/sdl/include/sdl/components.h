//---------------------------------------------------------------------------

#pragma once

#ifndef COMPONENTS_H
#define COMPONENTS_H

//---------------------------------------------------------------------------

#include <SDL.h>
#include <opengl/opengl.h>
#include <math/rect.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
    namespace sdl
    {
        class exception : public std::runtime_error
        {
        public:
            exception(const std::string & desc) : runtime_error(desc + " SDL error: " + SDL_GetError()) {}
        };
    
        class glew_exception : public std::runtime_error
        {
        public:
            glew_exception(GLenum error, const std::string & desc) : runtime_error(desc + " GLEW error: " + reinterpret_cast<const char *>(glewGetErrorString(error))) {}
        };
    
        struct app
        {
            app() {
                if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                    throw exception("Couldn't initialize SDL!");
                }
            }
            
            ~app() {
                SDL_Quit();
            }
        };
        
        using window_size = math::int_size;
        
        class window
        {
        public:
            window(const sdl::app &, const window_size & size) : _size_source(size), _size_input(_size_source.get_observable()) {
                handle = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
                
                if (handle == nullptr) {
                    throw exception("Couldn't create window!");
                }
            }
            
            ~window() {
                if (handle != nullptr) {
                    SDL_DestroyWindow(handle);
                }
            }

            window_size size() const {
                return _size_source.get_value();
            }
            
            void set_size(const window_size & size) {
                _size_source.get_subscriber().on_next(size);
            }
            
            operator SDL_Window *() {
                return handle;
            }
            
            SDL_Window * operator ->() {
                return handle;
            }
            
            SDL_Window * handle = nullptr;

            const rxcpp::observable<window_size> & size_input() const {
                return _size_input;
            }
            
        protected:
            rx::behavior<window_size> _size_source;
            rx::observable<window_size> _size_input;
        };
        
        class surface
        {
        public:
            surface(sdl::window & window) {
                handle = SDL_GetWindowSurface(window);
                
                if (handle == nullptr) {
                    throw exception("Couldn't create window surface!");
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
            
            bool operator ==(SDL_Surface * h) const {
                return handle == h;
            }
            
            bool operator !=(SDL_Surface * h) const {
                return handle != h;
            }
            
            operator SDL_Surface *() {
                return handle;
            }
            
            SDL_Surface * operator ->() {
                return handle;
            }
        
        private:
            SDL_Surface * handle = nullptr;
        };
        
        class graphics : public opengl::context
        {
        public:
            graphics(opengl::driver & driver, sdl::window & window) : opengl::context(driver), window(window) {
                context = SDL_GL_CreateContext(window);
                
                if (context == nullptr) {
                    throw exception("Can't create graphics");
                }
                
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
                
                glewExperimental = GL_TRUE;
                auto error_code = glewInit();
                
                if (error_code != GLEW_OK) {
                    throw glew_exception(error_code, "Error initializing GLEW!");
                }
                
                if (SDL_GL_SetSwapInterval(1) < 0) {    // enable v-sync
                    throw exception("Warning: Unable to set v-sync!");
                }

                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                
                window.size_input().subscribe([=](const window_size & size) {
                    glViewport(0, 0, size.x, size.y);
                });
            }
            
            ~graphics() {
                if (!context) {
                    SDL_GL_DeleteContext(context);
                }
            }
            
            void swap() {
                SDL_GL_SwapWindow(window);
            }
            
            operator SDL_GLContext() {
                return context;
            }
            
            SDL_GLContext operator ->() {
                return context;
            }
        
        private:
            SDL_GLContext context;
            sdl::window window;
        };
    }
}

//---------------------------------------------------------------------------
#endif
