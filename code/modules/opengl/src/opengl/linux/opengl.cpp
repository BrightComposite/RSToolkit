//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <opengl/mesh.h>
#include <opengl/shader.h>

#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		static inline void GLAPIENTRY glDebugCallbackFunc(uint source, uint type, uint id, uint severity, int length, const char * message, const void * userParam);

		static GLXFBConfig choose_fb_config(::Display * display, int screen) {
			int attributes[] {
				GLX_X_RENDERABLE, True,
				GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
				GLX_RENDER_TYPE, GLX_RGBA_BIT,
				GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
				GLX_RED_SIZE, 8,
				GLX_GREEN_SIZE, 8,
				GLX_BLUE_SIZE, 8,
				GLX_ALPHA_SIZE, 8,
				GLX_DEPTH_SIZE, 24,
				GLX_STENCIL_SIZE, 8,
				GLX_DOUBLEBUFFER, True,
				GLX_SAMPLE_BUFFERS, 1,
				GLX_SAMPLES, 4,
				0
			};
			
			GLXFBConfig ret = 0;
			
			int count = 0;
			auto fbc = glXChooseFBConfig(display, screen, attributes, &count);
			
			if (fbc) {
				if (count >= 1) {
					ret = fbc[0];
				}
				
				XFree(fbc);
			}
			
			return ret;
		}
		
		window_context::window_context(opengl::driver & driver, window & w) : base(driver), ::asd::window_context(w) {
			init_device();
			
			auto update_size = [](const math::int_size & size) {
				glViewport(0, 0, size.x, size.y);
				glScissor(0, 0, size.x, size.y);
			};
			
			w.events.size.subscribe(update_size);
			update_size(w.size());
		}
		
		window_context::window_context(window_context && ctx) noexcept :
			base(std::forward<window_context>(ctx)), ::asd::window_context(std::forward<window_context>(ctx)) {}
		
		window_context::~window_context() {
			if (_window.display() != nullptr) { 							// if window display is not closed yet
				glXMakeCurrent(_window.display(), 0, 0);
				
				if (_context != nullptr) {									// if rendering context was created successfully
					glXDestroyContext(_window.display(), _context);
					_context = nullptr;										// clear context pointer
				}
			}
		}
		
		void window_context::flush() {
			glXSwapBuffers(_window.display(), _window.handle());
		}
		
		void window_context::prepare() {
			if (glXMakeCurrent(_window.display(), _window.handle(), _context) == GL_FALSE) {
				throw context_creation_exception("Can't bind OpenGL render context!");
			}
			
			if (!glXIsDirect(_window.display(), glXGetCurrentContext())) {
				std::cout << "Indirect GLX rendering context obtained" << std::endl;
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
#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
			
			static int att[] = {
				GLX_RGBA,
				GLX_RED_SIZE, 8,
				GLX_GREEN_SIZE, 8,
				GLX_BLUE_SIZE, 8,
				GLX_DEPTH_SIZE, 24,
				GLX_SAMPLE_BUFFERS, 1,
				GLX_SAMPLES, 4,
				GLX_DOUBLEBUFFER, True,
				0
			};
			
			auto screen = DefaultScreen(_window.display());
			auto root = RootWindow(_window.display(), screen);
			
			_visual_info = glXChooseVisual(_window.display(), screen, att);
			
			if (!_visual_info) {
				throw context_creation_exception("Failed to get visual info!");
			}
			
			::Window windowHandle = 0;
			
			{
				XSetWindowAttributes winAttr;
				
				winAttr.event_mask = StructureNotifyMask | KeyPressMask;
				winAttr.background_pixmap = 0;
				winAttr.background_pixel = 0;
				winAttr.border_pixel = 0;
				
				winAttr.colormap = XCreateColormap(_window.display(), root, _visual_info->visual, AllocNone);
				
				unsigned int mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;
				
				windowHandle = XCreateWindow(_window.display(), root, 0, 0, 100, 100, 0, _visual_info->depth, InputOutput, _visual_info->visual, mask, &winAttr);
				
				if (!windowHandle) {
					throw context_creation_exception("Cannot create X Window!");
				}
			}
			
			GLXContext _tmpContext = glXCreateContext(_window.display(), _visual_info, 0, True);
			
			if (!_tmpContext) {
				throw context_creation_exception("Could not allocate temporary OpenGL render context!");
			}
			
			if (glXMakeCurrent(_window.display(), windowHandle, _tmpContext) == GL_FALSE) {
				throw context_creation_exception("Can't bind temporary OpenGL render context!");
			}
			
			glewExperimental = GL_TRUE;
			GLenum error = glewInit();
			
			if (error > 0) {
				throw context_creation_exception("Can't initialize GLEW! ", (const char *) glewGetErrorString(error));
			}
			
			glXMakeCurrent(_window.display(), 0, 0);
			glXDestroyContext(_window.display(), _tmpContext);
			
			auto fbConfig = choose_fb_config(_window.display(), screen);
			
			if (!fbConfig) {
				throw context_creation_exception("Failed to get GLXFBConfig!");
			}
			
			XFree(_visual_info);
			_visual_info = glXGetVisualFromFBConfig(_window.display(), fbConfig);
			
			if (!_visual_info) {
				throw context_creation_exception("Failed to get visual info!");
			}
			
			XDestroyWindow(_window.display(), windowHandle);
			
			int flags = _driver.config.flags;

#ifdef GL_DEBUG
			
			flags |= GLX_CONTEXT_DEBUG_BIT_ARB;

#endif
			
			const int attributes[] = {
				GLX_CONTEXT_MAJOR_VERSION_ARB, _driver.config.major,
				GLX_CONTEXT_MINOR_VERSION_ARB, _driver.config.minor,
				GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			#ifdef GL_DEBUG
				GLX_CONTEXT_FLAGS_ARB, flags,
			#endif
				0
			};
			
			_context = glXCreateContextAttribsARB(_window.display(), fbConfig, 0, True, attributes);
			XSync(_window.display(), False);
			
			if (!_context) {
				throw context_creation_exception("Can't create OpenGL render context!");
			}
		}
		
		driver::driver(const configuration & config) : config(config) {
			// register_method(opengl::draw_mesh);
		}
	}

//---------------------------------------------------------------------------
	
	namespace gfx
	{
		driver_context<opengl::driver>::driver_context(opengl::driver & d) : _driver(d) {}
		driver_context<opengl::driver>::driver_context(driver_context && ctx) : _driver(ctx._driver), _context(ctx._context) {
			ctx._context = nullptr;
		}
		
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
