//---------------------------------------------------------------------------

#ifndef OPENGL_3_3_H
#define OPENGL_3_3_H

//---------------------------------------------------------------------------

#include <core/addition/cached.h>
#include <core/addition/singleton.h>
#include <container/ArrayList.h>

#include <message/Subject.h>

#include <graphics/Graphics3D.h>

#include <ui/WidgetMessages.h>

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

	namespace OpenGL3_3
	{
		class GLGraphics;
		class GLImage;
		class GLShader;
		class GLShaderProgram;
	}

	namespace OpenGL3_3
	{
	#if BOOST_OS_WINDOWS
		using DeviceContext = HDC;
		using Context = HGLRC;
		using WindowHandle = HWND;
	#elif BOOST_OS_LINUX
		using Context = ::GLXContext;
		using WindowHandle = ::Window;
	#endif

		struct GLUniformBinding
		{
			GLUniformBinding(const char * name, int index) : name(name), index(index) {}

			const char * name;
			int index;
		};

		//---------------------------------------------------------------------------

		class GLGraphics : public Graphics3D
		{
			friend_graphics_provider(GLGraphics);
			friend class GLVertexLayout;
			friend class GLShaderProgram;

		public:
			struct ContextAttributes
			{
				ContextAttributes(int major, int minor, int flags) : major(major), minor(minor), flags(flags) {}
				ContextAttributes() : ContextAttributes(0, 0, 0) {}
				ContextAttributes(const ContextAttributes & a) : ContextAttributes(a.major, a.minor, a.flags) {}

				int major;
				int minor;
				int flags;
			};

			using Graphics3D::init;
			using Graphics3D::bind;
			using Graphics3D::draw;

			api(opengl3_3) void bind(const GLShaderProgram * program);

			virtual api(opengl3_3) void bind(const handle<Texture> & texture, uint index) override;

			virtual api(opengl3_3) void clip(const int_rect & rect) override;
			virtual api(opengl3_3) void present() const override;

			virtual api(opengl3_3) void printInfo();
			virtual api(opengl3_3) void printDebug();
			virtual api(opengl3_3) void checkForErrors() override;

			virtual api(opengl3_3) handle<asd::image> createImage(const image_data & data) override;
			virtual api(opengl3_3) handle<asd::surface> createSurface(ui_space * space) override;
			virtual api(opengl3_3) handle<TextureSurface> createSurface(const int_size & size) override;

			virtual api(opengl3_3) handle<vertex_buffer> createVertexBuffer(vertex_layout * layout, const vertex_data & data) override;
			virtual api(opengl3_3) handle<mesh_builder> createMesh() override;

			virtual api(opengl3_3) void addShaderProgram(const string & id, vertex_layout * layout, ShaderCodeSet & codeSet) override;

			virtual handle<GraphicsDebug> getDebug() const override
			{
				return nullptr;
			}

			Context context() const
			{
				return _context;
			}

#if BOOST_OS_WINDOWS
			static api(opengl3_3) void setPixelFormat(HDC dc);
#endif

		protected:
			api(opengl3_3) GLGraphics();
			virtual api(opengl3_3) ~GLGraphics();

			virtual api(opengl3_3) handle<UniformAdapter> & init(handle<UniformAdapter> & adapter, const char * name, ShaderType shader, int index, size_t size) override;

			virtual api(opengl3_3) void initFacilities() override;

		private:
			api(opengl3_3) void initDevice();
			api(opengl3_3) void initShaders();
		
			WindowHandle _wnd;
#if BOOST_OS_WINDOWS
			DeviceContext _device;
#endif
			Context _context;

			array_list<GLUniformBinding> _uniformBindings;
			const GLShaderProgram * _shaderProgram = nullptr;
		};
	}

	inline int glGetInteger(uint param)
	{
		int value;
		glGetIntegerv(param, &value);
		return value;
	}

#ifndef OpenGL
#define OpenGL OpenGL3_3
	using OpenGL3_3::GLGraphics;
	using GraphicsProvider = CommonGraphicsProvider<GLGraphics>;
#endif
}

//---------------------------------------------------------------------------
#endif
