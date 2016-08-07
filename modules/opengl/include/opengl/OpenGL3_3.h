//---------------------------------------------------------------------------

#ifndef OPENGL_3_3_H
#define OPENGL_3_3_H

//---------------------------------------------------------------------------

#include <core/addition/Cached.h>
#include <core/addition/Singleton.h>
#include <core/container/ArrayList.h>

#include <message/Subject.h>

#include <graphics/Graphics3D.h>

#include <ui/WidgetMessages.h>

//---------------------------------------------------------------------------

#define GLEW_STATIC

#include <gl/glew.h>

#ifdef _WIN32
#include <gl/wglew.h>
#else
#include <gl/glxew.h>
#endif // _WIN32

#ifdef _DEBUG
#define GL_DEBUG
#endif

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::thread;

	namespace OpenGL3_3
	{
		class GLGraphics;
		class GLImage;
		class GLShader;
		class GLShaderProgram;
		class GLMesh;
	}

	link_class(opengl3_3, OpenGL3_3::GLGraphics, Class<Graphics3D>);
	
	namespace OpenGL3_3
	{
	#ifdef _WIN32
		typedef HGLRC Context;
	#else
		typedef GLXContext Context;
	#endif // _WIN32

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

			virtual api(opengl3_3) void clip(const IntRect & rect) override;
			virtual api(opengl3_3) void present() const override;

			virtual api(opengl3_3) void printInfo();
			virtual api(opengl3_3) void printDebug();
			virtual api(opengl3_3) void checkForErrors() override;

			virtual api(opengl3_3) Handle<Image> createImage(const ImageData & data) override;
			virtual api(opengl3_3) Handle<Surface> createSurface(UISpace * space) override;
			virtual api(opengl3_3) Handle<Surface> createSurface(const IntSize & size, Handle<Image> & image) override;

			virtual api(opengl3_3) Handle<VertexBuffer> createVertexBuffer(VertexLayout * layout, const VertexData & data) override;
			virtual api(opengl3_3) Handle<Mesh> createMesh() override;

			virtual api(opengl3_3) void addShaderProgram(const string & id, VertexLayout * layout, ShaderCodeSet & codeSet) override;

			virtual Handle<GraphicsDebug> getDebug() const override
			{
				return nullptr;
			}

			Context context() const
			{
				return _context;
			}

			static api(opengl3_3) void setPixelFormat(HDC dc);

		protected:
			api(opengl3_3) GLGraphics();
			virtual api(opengl3_3) ~GLGraphics();

			virtual api(opengl3_3) Handle<UniformAdapter> & init(Handle<UniformAdapter> & adapter, const char * name, ShaderType shader, int index, size_t size) override;

			virtual api(opengl3_3) void initFacilities() override;

		private:
			api(opengl3_3) void initDevice();
			api(opengl3_3) void initShaders();

			HWND _wnd;
			HDC _device;
			Context _context;

			array_list<GLUniformBinding> _uniformBindings;
			const GLShaderProgram * _shaderProgram = nullptr;
			const GLMesh * _mesh = nullptr;
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
