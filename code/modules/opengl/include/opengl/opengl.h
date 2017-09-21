//---------------------------------------------------------------------------

#ifndef OPENGL_H
#define OPENGL_H

//---------------------------------------------------------------------------

#include <container/array_list.h>

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
	}
	
	namespace gfx
	{
		template <>
		class driver_context<opengl::driver> : public context
		{
			using entry_type = pair<morph_id_t, primitive>;
			using modifier_type = gfx::modifier<opengl::driver>;

			friend gfx::driver<opengl::driver>;

		public:
			api(opengl)
			driver_context(opengl::driver & driver);

			api(opengl)
			virtual ~driver_context();

			api(opengl)
			void draw();

			template<class Modifier>
			void apply() {
				auto i = _modifiers.find(gfx::modifier_id<Modifier>);

				if(i == _modifiers.end()) {
#ifdef GFX_IGNORE_MISSING_MODIFIERS
					return;
#else
					throw Exception("Modifier not found: ", Modifier::key());
#endif
				}

				i->second->apply(*this);
			}

		protected:
			api(opengl)
			void check_for_errors();

			void extend(morph_id_t id, unique<modifier_type> && m) {
				_modifiers.emplace(std::make_pair(id, forward<unique<modifier_type>>(m)));
			}

			opengl::driver & _driver;
			GLContext _context;
			map<morph_id_t, unique<modifier_type>> _modifiers;
		};
	}
	
	namespace opengl
	{
		using context = gfx::driver_context<driver>;
		using modifier = gfx::modifier<driver>;
		
		class window_context : public context, public ::asd::window_context
		{
			using base = opengl::context;
		
		public:
			api(opengl)
			window_context(opengl::driver & driver, window & w);

			api(opengl)
			virtual ~window_context();

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

			template<class Shader>
			driver & shader() {
				modifier<Shader>([](opengl::context & ctx) {
					return make::unique<opengl::shader_program>(ctx, opengl::shader_code::get(Shader::key()));
				});

				return *this;
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
