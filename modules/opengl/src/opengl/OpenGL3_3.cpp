//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>

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

				default:
					sev = String::hex(severity);
			}

			switch(type)
			{
				case GL_DEBUG_TYPE_ERROR:
					t = "Error";
					break;

				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
					t = "Deprecated behavior";
					break;

				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
					t = "Undefined behavior";
					break;

				case GL_DEBUG_TYPE_PORTABILITY:
					t = "Portability";
					break;

				case GL_DEBUG_TYPE_PERFORMANCE:
					t = "Performance";
					break;

				case GL_DEBUG_TYPE_OTHER:
					t = "Other";
					break;

				case GL_DEBUG_TYPE_MARKER:
					t = "Marker";
					break;

				case GL_DEBUG_TYPE_PUSH_GROUP:
					t = "Push group";
					break;

				case GL_DEBUG_TYPE_POP_GROUP:
					t = "Pop group";
					break;

				default:
					t = String::assemble("Unknown (", String::hex(type), ")");
			}

			switch(source)
			{
				case GL_DEBUG_SOURCE_API:
					src = "API";
					break;

				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
					src = "Window system";
					break;

				case GL_DEBUG_SOURCE_SHADER_COMPILER:
					src = "Shader compiler";
					break;

				case GL_DEBUG_SOURCE_THIRD_PARTY:
					src = "3rd party";
					break;

				case GL_DEBUG_SOURCE_APPLICATION:
					src = "Application";
					break;

				default:
					src = String::assemble("Unknown (", String::hex(source), ")");
			}

			//if(severity != 0)
			{
				using namespace std;

				cout << "--" << endl;
				cout << "OpenGL debugger message:" << endl;
				cout << "\tGROUP: " << t << ", SEVERITY: " << sev << ", ID: 0x" << hex << id << dec << ", SOURCE: " << src << endl;
				cout << "\tTEXT: \"" << message << "\"" << endl << endl;
			}
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

			checkForErrors();

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glEnable(GL_CULL_FACE);
			//glEnable(GL_SCISSOR_TEST);

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

		void GLGraphics::bind(const GLMeshTrait * mesh)
		{
			if(_mesh != mesh)
			{
				_mesh = mesh;
				glBindVertexArray(mesh != nullptr ? mesh->id : 0);
			}
		}

		void GLGraphics::printInfo() {}
		void GLGraphics::printDebug() {}

		void GLGraphics::checkForErrors()
		{
			GLenum error = glGetError();

			if(error == GL_NO_ERROR)
				return;

			if(glErrorStringREGAL != nullptr)
				throw Exception("OpenGL error: ", glErrorStringREGAL(error));
			else
				throw Exception("OpenGL error, code: ", String::hex(error));
		}

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

		Handle<Mesh> GLGraphics::createMesh(const Handle<VertexBuffer> & buffer, VertexTopology topology, uint verticesLocation)
		{
			return Handle<GLMesh>(this, buffer, topology, verticesLocation);
		}

		Handle<IndexedMesh> GLGraphics::createMesh(const Handle<VertexBuffer> & buffer, const VertexIndices & indices, VertexTopology topology, uint verticesLocation, uint indicesLocation)
		{
			return Handle<GLIndexedMesh>(this, buffer, createIndexBuffer(indices), topology, verticesLocation, indicesLocation);
		}

		UniqueHandle<UniformAdapter> GLGraphics::createUniformAdapter(const char * name, ShaderType shader, int index, size_t size)
		{
			_uniformBindings.emplace_back(name, index);

			for(auto & p : _shaderPrograms)
			{
				auto program = handle_cast<GLShaderProgram>(valueof(p));
				auto block_index = glGetUniformBlockIndex(program->id, name);

				if(block_index != GL_INVALID_INDEX)
					glUniformBlockBinding(program->id, block_index, index);
			}

			checkForErrors();

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
