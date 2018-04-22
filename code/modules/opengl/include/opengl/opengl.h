//---------------------------------------------------------------------------

#ifndef OPENGL_H
#define OPENGL_H

//---------------------------------------------------------------------------

#include <container/array_list.h>

#include <graphics/color.h>
#include <graphics/graphics.h>
#include <window/window.h>

#include <thread>

//---------------------------------------------------------------------------

#if BOOST_OS_WINDOWS

#include <GL/glew.h>
#include <GL/wglew.h>

#elif BOOST_OS_LINUX

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include <GL/glxew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <container/any_list.h>

#endif

#if ASD_DEBUG

#define GL_DEBUG

#endif

#undef major
#undef minor

//---------------------------------------------------------------------------

namespace asd
{
    using std::thread;

#if BOOST_OS_WINDOWS

    using DeviceContext = HDC;
    using GLContext = HGLRC;

#elif BOOST_OS_LINUX

    using GLContext = ::GLXContext;

#endif

    namespace opengl
    {
        class driver;
        struct shader_program;
    }

    namespace gfx
    {
        template <>
        class driver_context<opengl::driver> : public context
        {
            friend gfx::driver<opengl::driver>;

        public:
            api(opengl)
            driver_context(opengl::driver & driver);

            api(opengl)
            virtual ~driver_context();

            api(opengl)
            void check_for_errors();

        protected:
            opengl::driver & _driver;
        };
    }

    namespace opengl
    {
        using context = gfx::driver_context<driver>;

        class driver : public ::asd::gfx::driver<driver>
        {
        public:
            struct configuration
            {
                configuration() {}
                configuration(int major, int minor, int flags) : major(major), minor(minor), flags(flags) {}
                configuration(const configuration & a) : configuration(a.major, a.minor, a.flags) {}

                int major = 3;
                int minor = 3;
                int flags = 0;
            };

            api(opengl)
            driver(const configuration & config = {});

            configuration config;
        };

        api(opengl)
        void GLAPIENTRY glDebugCallbackFunc(uint source, uint type, uint id, uint severity, int length, const char * message, const void * userParam);
    }

//---------------------------------------------------------------------------

    inline int glGetInteger(uint param) {
        int value;
        glGetIntegerv(param, &value);
        return value;
    }
}

//---------------------------------------------------------------------------
#endif // OPENGL_H
