//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <opengl/mesh.h>
#include <opengl/shader.h>

#include <iostream>

//---------------------------------------------------------------------------

#ifdef WIN32
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "winmm.lib")
#endif

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		static inline void GLAPIENTRY glDebugCallbackFunc(uint source, uint type, uint id, uint severity, int length, const char * message, const void * userParam);

		static void set_pixel_format(HDC dc)
		{
			if(dc == nullptr) {
				throw context_creation_exception("Can't set pixel format, device context is null!");
			}
			
			PIXELFORMATDESCRIPTOR pfd;

			const int attribs[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_COLOR_BITS_ARB, 32,
				WGL_DEPTH_BITS_ARB, 24,
				WGL_STENCIL_BITS_ARB, 8,
				0
			};

			int pixelFormat, numFormats;
			wglChoosePixelFormatARB(dc, attribs, NULL, 1, &pixelFormat, (UINT*)&numFormats);

			if(SetPixelFormat(dc, pixelFormat, &pfd) == FALSE) {
				throw context_creation_exception("Can't set pixel format! Call to SetPixelFormat failed");
			}
		}

		window_context::~window_context() {
			wglMakeCurrent(nullptr, nullptr);

			if(_context != nullptr) {
				wglDeleteContext(_context);
			}
		}
		
		void window_context::flush() {
			SwapBuffers(_window.device());
		}
		
		void window_context::prepare() {
			set_pixel_format(_window.device());

			int flags = _driver.config.flags;

#ifdef GL_DEBUG

			flags |= WGL_CONTEXT_DEBUG_BIT_ARB;

#endif

			int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, _driver.config.major,
				WGL_CONTEXT_MINOR_VERSION_ARB, _driver.config.minor,
				WGL_CONTEXT_FLAGS_ARB,         flags,
				WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0
			};

			_context = wglCreateContextAttribsARB(_window.device(), nullptr, attribs);

			if(!_context) {
				throw context_creation_exception("Can't create OpenGL render context!");
			}

			if(wglMakeCurrent(_window.device(), _context) == GL_FALSE) {
				throw context_creation_exception("Can't bind OpenGL render context!");
			}

			check_for_errors();
			
			glDisable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);
			glEnable(GL_SCISSOR_TEST);
			
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

#ifdef GL_DEBUG
			
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			
			if(glDebugMessageCallback != nullptr) {
				glDebugMessageCallback(glDebugCallbackFunc, nullptr);
			} else {
				std::cout << "glDebugMessageCallback is not supported" << std::endl;
			}

#endif
			
			check_for_errors();
			
			// _driver.set_modifiers(*this);
		}

		void window_context::init_device() {
			auto _wnd = window::create_empty_handle();
			auto _device = ::GetDC(_wnd);

			PIXELFORMATDESCRIPTOR pfd;
			memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

			pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion   = 1;
			pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 32;
			pfd.iLayerType = PFD_MAIN_PLANE;

			int pixelFormat = ChoosePixelFormat(_device, &pfd);

			if(pixelFormat == 0) {
				throw context_creation_exception("Can't choose pixel format!");
			}

			if(SetPixelFormat(_device, pixelFormat, &pfd) == FALSE) {
				throw context_creation_exception("Can't set pixel format! Call to SetPixelFormat failed");
			}

			auto _tmpContext = wglCreateContext(_device);

			if(_tmpContext == nullptr || wglMakeCurrent(_device, _tmpContext) == GL_FALSE) {
				throw context_creation_exception("Can't create initial OpenGL render context!");
			}

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();

			if(error > 0) {
				throw context_creation_exception("Can't initialize GLEW! ", (const char *)glewGetErrorString(error));
			}

			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(_tmpContext);
			DestroyWindow(_wnd);
		}

		driver::driver(const configuration & config) : config(config) {
			// register_method(opengl::draw_mesh);
		}
		
		window_context::window_context(opengl::driver & driver, window & w) : base(driver), ::asd::window_context(w) {
			init_device();
			
			auto update_size = [](const math::int_size & size) {
				glViewport(0, 0, size.x, size.y);
				glScissor(0, 0, size.x, size.y);
			};

			w.inputs.size.subscribe(update_size);
			update_size(w.size());
		}
	}

//---------------------------------------------------------------------------
	
	namespace gfx
	{
		driver_context<opengl::driver>::driver_context(opengl::driver & d) : _driver(d) {}
		
		driver_context<opengl::driver>::~driver_context() {}
		
		void driver_context<opengl::driver>::check_for_errors() {
			GLenum error = glGetError();
			
			if (error != GL_NO_ERROR) {
				throw context_creation_exception("opengl error, code: ", String::hex(error));
			}
		}
		
		/*
		handle<image> GLGraphics::createImage(const image_data & data) {
			return handle<GLImage>(this, data);
		}
		
		handle<vertex_buffer> GLGraphics::createVertexBuffer(vertex_layout * layout, const vertex_data & data) {
			return handle<GLVertexBuffer>(this, layout, data);
		}
		
		handle<mesh_builder> GLGraphics::createMesh() {
			return handle<mesh_builder>(this);
		}
		
		handle<UniformAdapter> & GLGraphics::init(handle<UniformAdapter> & adapter, const char * name, ShaderType shader, int index, size_t size) {
			_uniformBindings.emplace_back(name, index);
			
			for(auto & p : _shaderPrograms) {
				auto id = cast::as<GLShaderProgram>(valueof(p))->id;
				auto block = glGetUniformBlockIndex(id, name);
				
				if(block != GL_INVALID_INDEX)
					glUniformBlockBinding(id, block, index);
			}
			
			checkForErrors();
			
			adapter = handle<GLUniformAdapter>(this, shader, index, static_cast<uint>(size));
			return adapter;
		}
		
		handle<asd::surface> GLGraphics::createSurface(ui_space * space) {
			return handle<UISurface>(this, space);
		}
		
		handle<TextureSurface> GLGraphics::createSurface(const int_size & size) {
			return handle<GLTextureSurface>(this, size);
		}
		
		void GLGraphics::addShaderProgram(const string & id, vertex_layout * layout, ShaderCodeSet & codeSet) {
			_shaderPrograms[id] = handle<GLShaderProgram>(this, id, layout, codeSet);
		}
		
		void GLGraphics::present() const {
			_surface->present();
		}
		
		void GLGraphics::clip(const int_rect & rect) {
			if(_clipRect == rect) {
				return;
			}
			
			_clipRect = rect;
			glScissor(_clipRect.left, surface()->height() - _clipRect.bottom, _clipRect.width(), _clipRect.height());
		}*/
	}
	
	namespace opengl
	{
		static inline void GLAPIENTRY glDebugCallbackFunc(uint source, uint type, uint id, uint severity, int length, const char * message, const void * userParam) {
			string sev, t, src;
			
			switch (severity) {
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
			
			switch (type) {
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
			
			switch (source) {
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
			cout << "\tgroup: " << t << ", severity: " << sev << ", id: " << hex << showbase << id << dec
				<< ", source: " << src << endl;
			cout << message << endl << endl;
			
			if (severity == GL_DEBUG_SEVERITY_HIGH) {
				throw Exception("GL error!");
			}
		}
	}
}
