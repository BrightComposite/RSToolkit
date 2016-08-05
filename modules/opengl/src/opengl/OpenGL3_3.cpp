//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>
#include <opengl/GLObjects.h>

#include <ui/UISpace.h>

#include <iostream>

//---------------------------------------------------------------------------

#pragma comment(lib, "opengl32.lib")
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

		class LineWidthState : public Graphics::BrushState<int>
		{
			using Base = typename Graphics::BrushState<int>;

		public:
			LineWidthState(GLGraphics * graphics) : Base(graphics, 1) {}

		protected:
			virtual void change() override
			{
				Base::change();
				glLineWidth(float(_state));
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

				graphics->setDepth(0.0f);
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
				glClearColor(_state.r, _state.g, _state.b, _state.a);
			}
		};

		GLGraphics::GLGraphics()
		{
			setclass(GLGraphics);

			initDevice();

			_fillMode = handle<FillModeState>(this);
			_lineWidth = handle<LineWidthState>(this);
			_depthTestMode = handle<DepthTestState>(this);
			_blendMode = handle<BlendState>(this);
			_clearColor = handle<ClearColorState>(this);
		}

		GLGraphics::~GLGraphics()
		{
			wglMakeCurrent(nullptr, nullptr);

			if(_context != nullptr)
			{
				wglMakeCurrent(nullptr, nullptr);
				wglDeleteContext(_context);
			}

			if(_device != nullptr)
			{
				::ReleaseDC(_wnd, _device);
				::DeleteDC(_device);
			}

			if(_wnd != nullptr)
				::DestroyWindow(_wnd);
		}

		void GLGraphics::setPixelFormat(HDC dc)
		{
			if(dc == nullptr)
				throw Exception("Can't set pixel format, device context is null!");

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
					throw Exception("Can't describe pixel format!");
			}

			if(::SetPixelFormat(dc, format, &pfd) != TRUE)
				throw Exception("Can't set pixel format! Call to SetPixelFormat failed");
		}

		void APIENTRY glDebugCallbackFunc(uint source, uint type, uint id, uint severity, int length, const char * message, void * userParam)
		{
			string sev, t, src;

			switch(severity)
			{
				case GL_DEBUG_SEVERITY_LOW:
					sev = "low";
					break;

				case GL_DEBUG_SEVERITY_MEDIUM:
					sev = "medium";
					break;

				case GL_DEBUG_SEVERITY_HIGH:
					sev = "high";
					break;

				case GL_DEBUG_SEVERITY_NOTIFICATION:
					return;
					//sev = "notification";
					//break;

				default:
					sev = String::hex(severity);
			}

			switch(type)
			{
				case GL_DEBUG_TYPE_ERROR:
					t = "error";
					break;

				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
					t = "deprecated behavior";
					break;

				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
					t = "undefined behavior";
					break;

				case GL_DEBUG_TYPE_PORTABILITY:
					t = "portability";
					break;

				case GL_DEBUG_TYPE_PERFORMANCE:
					t = "performance";
					break;

				case GL_DEBUG_TYPE_OTHER:
					t = "other";
					break;

				case GL_DEBUG_TYPE_MARKER:
					t = "marker";
					break;

				case GL_DEBUG_TYPE_PUSH_GROUP:
					t = "push group";
					break;

				case GL_DEBUG_TYPE_POP_GROUP:
					t = "pop group";
					break;

				default:
					t = String::assemble("unknown (", String::hex(type), ")");
			}

			switch(source)
			{
				case GL_DEBUG_SOURCE_API:
					src = "API";
					break;

				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
					src = "window system";
					break;

				case GL_DEBUG_SOURCE_SHADER_COMPILER:
					src = "shader compiler";
					break;

				case GL_DEBUG_SOURCE_THIRD_PARTY:
					src = "3rd party";
					break;

				case GL_DEBUG_SOURCE_APPLICATION:
					src = "application";
					break;

				default:
					src = String::assemble("unknown (", String::hex(source), ")");
			}

			using namespace std;

			cout << "--------------------------------" << endl;
			cout << "OpenGL debug:" << endl;
			cout << "\tgroup: " << t << ", severity: " << sev << ", id: " << hex << showbase << id << dec << ", source: " << src << endl;
			cout << message << endl << endl;
		}

		void GLGraphics::initDevice()
		{
			_wnd = createEmptyWindowHandle();
			_device = ::GetDC(_wnd);
			setPixelFormat(_device);

			auto _tmpcontext = wglCreateContext(_device);

			if(_tmpcontext == nullptr || wglMakeCurrent(_device, _tmpcontext) == GL_FALSE)
				throw Exception("Can't create initial OpenGL render context!");

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();

			if(error > 0)
				throw Exception("Can't initialize GLEW! ", (const char *)glewGetErrorString(error));

			int flags = 0;

		#ifdef GL_DEBUG
			flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
		#endif

			int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, glGetInteger(GL_MAJOR_VERSION),
				WGL_CONTEXT_MINOR_VERSION_ARB, glGetInteger(GL_MINOR_VERSION),
				WGL_CONTEXT_FLAGS_ARB,         flags,
				WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0
			};

			_context = wglCreateContextAttribsARB(_device, nullptr, attribs);

			if(_context == nullptr || wglMakeCurrent(_device, _context) == GL_FALSE)
				throw Exception("Can't create OpenGL render context!");

			wglDeleteContext(_tmpcontext);

			checkForErrors();

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);
			glEnable(GL_SCISSOR_TEST);

			glCullFace(GL_BACK);
			glFrontFace(GL_CW);

			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

		#ifdef GL_DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			if(glDebugMessageCallback != nullptr)
				glDebugMessageCallback(glDebugCallbackFunc, nullptr);
			else
			{
				using namespace std;
				cout << "glDebugMessageCallback is not supported" << endl;
			}
		#endif

			checkForErrors();
		}

		void GLGraphics::initFacilities()
		{
			initShaders();
			Graphics3D::initFacilities();
			checkForErrors();
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

		void GLGraphics::checkForErrors()
		{
			GLenum error = glGetError();

			if(error != GL_NO_ERROR)
				throw Exception("OpenGL error, code: ", String::hex(error));
		}

		Handle<Image> GLGraphics::createImage(const ImageData & data)
		{
			return Handle<GLImage>(this, data);
		}

		Handle<VertexBuffer> GLGraphics::createVertexBuffer(VertexLayout * layout, const VertexData & data)
		{
			return Handle<GLVertexBuffer, GLGraphics>(this, layout, data);
		}

		Handle<Mesh> GLGraphics::createMesh()
		{
			return Handle<GLMesh>(this);
		}

		Handle<UniformAdapter> & GLGraphics::init(Handle<UniformAdapter> & adapter, const char * name, ShaderType shader, int index, size_t size)
		{
			_uniformBindings.emplace_back(name, index);

			for(auto & p : _shaderPrograms)
			{
				auto id = handle_cast<GLShaderProgram>(valueof(p))->id;
				auto block = glGetUniformBlockIndex(id, name);

				if(block != GL_INVALID_INDEX)
					glUniformBlockBinding(id, block, index);
			}

			checkForErrors();

			adapter = Handle<GLUniformAdapter, GLGraphics>(this, shader, index, size);
			return adapter;
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
			glScissor(_clipRect.left, surface()->height() - _clipRect.bottom, _clipRect.width(), _clipRect.height());
		}
	}
}
