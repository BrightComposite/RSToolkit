//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>

#include <ui/UISpace.h>

//---------------------------------------------------------------------------

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "winmm.lib")

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(OpenGL3_3::GLGraphics);

	namespace OpenGL3_3
	{
		class FillModeState : public State<FillMode>
		{
		public:
			FillModeState(GLGraphics * graphics) : State<FillMode>(FillMode::Solid) {}

		protected:
			virtual void change() override
			{
				if(_state == FillMode::Solid)
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
		};

		class DepthTestState : public State<bool>
		{
		public:
			DepthTestState(GLGraphics * graphics) : State<bool>(true), graphics(graphics) {}

		protected:
			virtual void change() override
			{
				if(_state)
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);

				graphics->setDepth(1.0f);
			}

			GLGraphics * graphics;
		};

		class BlendState : public State<bool>
		{
		public:
			BlendState(GLGraphics * graphics) : State<bool>(true) {}

		protected:
			virtual void change() override
			{
				if(_state)
					glEnable(GL_BLEND);
				else
					glEnable(GL_BLEND);
			}
		};

		class ClearColorState : public State<Color>
		{
		public:
			ClearColorState(GLGraphics * graphics) : State<Color>(0.0f, 0.0f, 0.0f, 1.0f) {}

		protected:
			virtual void change() override
			{
				glClearColor(_state[0], _state[1], _state[2], _state[3]);
			}
		};

		GLGraphics::GLGraphics()
		{
			setclass(GLGraphics);

			initDevice();

			_fillMode = handle<FillModeState>(this);
			_depthTestMode = handle<DepthTestState>(this);
			_blendMode = handle<BlendState>(this);
			_clearColor = handle<ClearColorState>(this);
		}

		GLGraphics::~GLGraphics()
		{
			wglMakeCurrent(nullptr, nullptr);

			if(_context != nullptr)
				wglDeleteContext(_context);

			if(_device != nullptr)
			{
				::ReleaseDC(_wnd, _device);
				::DeleteDC(_device);
			}

			if(_wnd != nullptr)
				::DestroyWindow(_wnd);
		}

		static bool setPixelFormat(HDC dc)
		{
			if(dc == nullptr)
				return false;

			PIXELFORMATDESCRIPTOR pfd;
			memset(&pfd, 0, sizeof(pfd));

			pfd.nSize = sizeof(pfd);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 24;

			int format = ::ChoosePixelFormat(dc, &pfd);

			if(format == 0)
			{
				format = 1;

				if(::DescribePixelFormat(dc, format, sizeof(pfd), &pfd) == FALSE)
					return false;
			}

			return ::SetPixelFormat(dc, format, &pfd) == TRUE;
		}

		void GLGraphics::initDevice()
		{
			_wnd = createEmptyWindowHandle();
			_device = ::GetDC(_wnd);
			setPixelFormat(_device);

			_context = wglCreateContext(_device);

			if(_context == nullptr || wglMakeCurrent(_device, _context) == GL_FALSE)
				throw Exception("Can't create OpenGL render context!");

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();

			if(error > 0)
				throw Exception("Can't initialize GLEW! ", (const char *)glewGetErrorString(error));

			glDepthFunc(GL_LESS);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);
			glEnable(GL_SCISSOR_TEST);
		}

		void GLGraphics::initFacilities()
		{
			initShaders();
			Graphics3D::initFacilities();
		}

		void GLGraphics::bind(const GLShaderProgram * program)
		{
			if(_shaderProgram != program)
			{
				_shaderProgram = program;
				glUseProgram(program->id);
			}
		}

		void GLGraphics::printInfo() {}
		void GLGraphics::printDebug() {}
		void GLGraphics::checkForErrors() {}

		Handle<Image> GLGraphics::createImage(const ImageData & data)
		{
			return Handle<GLImage>(this, data);
		}

		Handle<VertexLayout> GLGraphics::createVertexLayout(const string & fingerprint)
		{
			return Handle<GLVertexLayout, GLGraphics>(this, fingerprint);
		}

		Handle<VertexBuffer> GLGraphics::createVertexBuffer(VertexLayout * layout, const VertexData & data)
		{
			return Handle<GLVertexBuffer, GLGraphics>(this, layout, data);
		}

		Handle<IndexBuffer> GLGraphics::createIndexBuffer(const VertexIndices & indices)
		{
			return Handle<GLIndexBuffer, GLGraphics>(this, indices);
		}

		UniqueHandle<UniformAdapter> GLGraphics::createUniformAdapter(ShaderType shader, int index, size_t size)
		{
			return UniqueHandle<GLUniformAdapter, GLGraphics>(this, shader, index, size);
		}

		Handle<Surface> GLGraphics::createSurface(UISpace * space)
		{
			return Handle<UISurface>(this, space);
		}

		Handle<Surface> GLGraphics::createSurface(const IntSize & size, Handle<Image> & image)
		{
			return Handle<TextureSurface>(this, size, image);
		}

		void GLGraphics::present() const
		{
			_surface->present();
		}

		void GLGraphics::clip(const IntRect & rect)
		{
			if(_clipRect == rect)
				return;

			_clipRect = rect;
			glScissor(_clipRect.left, _clipRect.top, _clipRect.width(), _clipRect.height());
		}
	}
}
