//---------------------------------------------------------------------------

#pragma once

#ifndef SDLPP_H
#define SDLPP_H

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
            deny_copy(window);

        public:
            api(sdlpp)
            window(const sdl::app &, const window_size & size);

            api(sdlpp)
            ~window();

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
            graphics(opengl::driver & driver, sdl::window & window) : opengl::context(driver), _window(window) {
                _context = SDL_GL_CreateContext(window);

                if (_context == nullptr) {
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
                if (!_context) {
                    SDL_GL_DeleteContext(_context);
                }
            }

            graphics(graphics && ctx) noexcept :
                opengl::context(ctx._driver),
                _context(ctx._context),
                _window(ctx._window)
            {
                ctx._context = 0;
            }

            graphics & operator = (graphics && ctx) noexcept
            {
                std::swap(_context, ctx._context);
                return *this;
            }

            bool operator == (const graphics & ctx) const {
                return this == &ctx;
            }

            void swap() {
                SDL_GL_SwapWindow(_window);
            }

            operator SDL_GLContext() {
                return _context;
            }

            SDL_GLContext operator ->() {
                return _context;
            }

        private:
            SDL_GLContext _context;
            sdl::window & _window;
        };
    }
}

//---------------------------------------------------------------------------
#endif
