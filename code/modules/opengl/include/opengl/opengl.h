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

#define GLEW_STATIC

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
		
		namespace shader_code {
			struct store;
			
			store & get(const std::string & name);
		}
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

		protected:
			api(opengl)
			void check_for_errors();

			opengl::driver & _driver;
			GLContext _context;
		};
	}
	
	namespace opengl
	{
		using context = gfx::driver_context<driver>;
		
		class window_context : public opengl::context, public ::asd::window_context
		{
			using base = opengl::context;
		
		public:
			api(opengl)
			window_context(opengl::driver & driver, window & w);

			api(opengl)
			virtual ~window_context();

			api(opengl)
			virtual void flush() override;

			api(opengl)
			virtual void prepare() override;
		
		protected:
			void init_device();
		};
		
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
			
			unique<window_context> create_context(window & w) {
				return new window_context(*this, w);
			}

			configuration config;
		};
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
