//---------------------------------------------------------------------------

#include <sdlpp/sdlpp.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace sdl
    {
        window::window(const sdl::app &, const window_size & size) : _size_source(size), _size_input(_size_source.get_observable()) {
            handle = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

            if (handle == nullptr) {
                throw exception("Couldn't create window!");
            }
        }

        window::~window() {
            if (handle != nullptr) {
                SDL_DestroyWindow(handle);
            }
        }
    }
}

//---------------------------------------------------------------------------

