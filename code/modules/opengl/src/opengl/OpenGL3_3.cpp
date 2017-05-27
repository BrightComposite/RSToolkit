//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>
#include <opengl/GLObjects.h>

#include <ui/UISpace.h>

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
	implement_link(OpenGL3_3::GLGraphics);
	
	namespace OpenGL3_3
	{
		class FillModeState : public State<FillMode>
		{
		public:
			FillModeState(GLGraphics * graphics) : State<FillMode>(FillMode::Solid) {}
		
		protected:
			virtual void change() override {
				if(_state == FillMode::Solid) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				} else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
			}
		};
		
		class LineWidthState : public Graphics::BrushState<int>
		{
			using Base = typename Graphics::BrushState<int>;
		
		public:
			LineWidthState(GLGraphics * graphics) : Base(graphics, 1) {}
		
		protected:
			virtual void change() override {
				Base::change();
				glLineWidth(float(_state));
			}
		};
		
		class DepthTestState : public State<bool>
		{
		public:
			DepthTestState(GLGraphics * graphics) : State<bool>(false), graphics(graphics) {}
		
		protected:
			virtual void change() override {
				if(_state) {
					glEnable(GL_DEPTH_TEST);
				} else {
					glDisable(GL_DEPTH_TEST);
				}
				
				graphics->setDepth(0.0f);
			}
			
			GLGraphics * graphics;
		};
		
		class BlendState : public State<bool>
		{
		public:
			BlendState(GLGraphics * graphics) : State<bool>(true) {}
		
		protected:
			virtual void change() override {
				if(_state) {
					glEnable(GL_BLEND);
				} else {
					glEnable(GL_BLEND);
				}
			}
		};
		
		class AccumulationState : public State<bool>
		{
		public:
			AccumulationState(GLGraphics * graphics) : State<bool>(false) {}
		
		protected:
			virtual void change() override {
				if(_state) {
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_ONE, GL_ONE);
				} else {
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}
			}
		};
		
		class ClearColorState : public State<Color>
		{
		public:
			ClearColorState(GLGraphics * graphics) : State<Color>(0.0f, 0.0f, 0.0f, 1.0f) {}
		
		protected:
			virtual void change() override {
				glClearColor(_state.r, _state.g, _state.b, _state.a);
			}
		};
		
		GLGraphics::GLGraphics() : Graphics3D() {
			setclass(GLGraphics);
			
			initDevice();
			
			_fillMode = Handle<FillModeState>(this);
			_lineWidth = Handle<LineWidthState>(this);
			_depthTestMode = Handle<DepthTestState>(this);
			_blendMode = Handle<BlendState>(this);
			_clearColor = Handle<ClearColorState>(this);
			_accumulationMode = Handle<AccumulationState>(this);
		}
		
		GLGraphics::~GLGraphics() {
#if BOOST_OS_WINDOWS
			wglMakeCurrent(nullptr, nullptr);

			if(_context != nullptr) {
				wglDeleteContext(_context);
			}

			if(_device != nullptr) {
				::ReleaseDC(_wnd, _device);
				::DeleteDC(_device);
			}

			if(_wnd != nullptr) {
				::DestroyWindow(_wnd);
			}
#elif BOOST_OS_LINUX
			glXMakeCurrent(_display, 0, 0);
			
			if(_context != nullptr) {
				glXDestroyContext(_display, _context);
				_context = nullptr;
			}
			
			if(_display != nullptr) {
				if(_wnd != 0) {
					XDestroyWindow(_display, _wnd);
					_wnd = 0;
				}
				
				XCloseDisplay(_display);
				_display = nullptr;
			}
#endif
		}

#if BOOST_OS_WINDOWS
		void GLGraphics::setPixelFormat(HDC dc)
		{
			if(dc == nullptr)
				throw Exception("Can't set pixel format, device context is null!");

			PIXELFORMATDESCRIPTOR pfd; 
			
			const int attribs[] = 
			{ 
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

			if(SetPixelFormat(dc, pixelFormat, &pfd) == FALSE)
				throw Exception("Can't set pixel format! Call to SetPixelFormat failed");
		}
#endif

#if BOOST_OS_WINDOWS
		static void APIENTRY glDebugCallbackFunc(uint source, uint type, uint id, uint severity, int length, const char * message, void * userParam)
#else
		
		static void glDebugCallbackFunc(uint source, uint type, uint id, uint severity, int length, const char * message, const void * userParam)
#endif
		{
			string sev, t, src;
			
			switch(severity) {
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
			
			switch(type) {
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
			
			switch(source) {
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
			
			if(severity == GL_DEBUG_SEVERITY_HIGH) {
				throw Exception("GL error!");
			}
		}

#if BOOST_OS_LINUX
		
		static GLXFBConfig chooseFBConfig(::Display * display, int screen) {
			static const int v_attribs[] = {
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
			
			int attribs[100];
			memcpy(attribs, v_attribs, sizeof(v_attribs));
			
			GLXFBConfig ret = 0;
			
			int count;
			auto fbc = glXChooseFBConfig(display, screen, attribs, &count);
			
			if(fbc) {
				if(count >= 1) {
					ret = fbc[0];
				}
				
				XFree(fbc);
			}
			
			return ret;
		}
#endif
		
		void GLGraphics::initDevice() {
#if BOOST_OS_WINDOWS
			_wnd = createEmptyWindowHandle();
			_device = ::GetDC(_wnd);

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

			if(pixelFormat == 0)
				throw Exception("Can't choose pixel format!");

			if(SetPixelFormat(_device, pixelFormat, &pfd) == FALSE)
				throw Exception("Can't set pixel format! Call to SetPixelFormat failed");

			auto _tmpcontext = wglCreateContext(_device);

			if(_tmpcontext == nullptr || wglMakeCurrent(_device, _tmpcontext) == GL_FALSE)
				throw Exception("Can't create initial OpenGL render context!");

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();

			if(error > 0)
				throw Exception("Can't initialize GLEW! ", (const char *)glewGetErrorString(error));
			
			int major = glGetInteger(GL_MAJOR_VERSION);
			int minor = glGetInteger(GL_MINOR_VERSION);

			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(_tmpcontext);
			DestroyWindow(_wnd);

			_wnd = createEmptyWindowHandle();
			_device = ::GetDC(_wnd);

			setPixelFormat(_device);

			int flags = 0;

#ifdef GL_DEBUG
			flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

			int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, major,
				WGL_CONTEXT_MINOR_VERSION_ARB, minor,
				WGL_CONTEXT_FLAGS_ARB,         flags,
				WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0
			};

			_context = wglCreateContextAttribsARB(_device, nullptr, attribs);

			if(_context == nullptr || wglMakeCurrent(_device, _context) == GL_FALSE)
				throw Exception("Can't create OpenGL render context!");
#elif BOOST_OS_LINUX
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
				GLX_DOUBLEBUFFER,
				0
			};
			
			_display = XOpenDisplay(NULL);
			
			if(_display == NULL) {
				throw Exception("Cannot connect to X server!");
			}
			
			auto screen = DefaultScreen(_display);
			_visualInfo = glXChooseVisual(_display, screen, att);
			
			if(!_visualInfo) {
				throw Exception("Failed to get visinfo!");
			}
			
			auto root = RootWindow(_display, screen);
			
			{
				XSetWindowAttributes winAttr;
				
				winAttr.event_mask = StructureNotifyMask | KeyPressMask;
				winAttr.background_pixmap = 0;
				winAttr.background_pixel = 0;
				winAttr.border_pixel = 0;
				
				winAttr.colormap = XCreateColormap(_display, root, _visualInfo->visual, AllocNone);
				
				unsigned int mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;
				
				_wnd = XCreateWindow(_display, root, 0, 0, 100, 100, 0, _visualInfo->depth, InputOutput, _visualInfo->visual, mask, &winAttr);
			}
			
			if(!_wnd) {
				throw Exception("Cannot create X Window!");
			}
			
			GLXContext _tmpcontext = glXCreateContext(_display, _visualInfo, 0, True);
			
			if(!_tmpcontext) {
				throw Exception("Could not allocate temporary OpenGL render context!");
			}
			
			if(glXMakeCurrent(_display, _wnd, _tmpcontext) == GL_FALSE) {
				throw Exception("Can't bind temporary OpenGL render context!");
			}
			
			glewExperimental = GL_TRUE;
			GLenum error = glewInit();
			
			if(error > 0) {
				throw Exception("Can't initialize GLEW! ", (const char *) glewGetErrorString(error));
			}
			
			glXMakeCurrent(_display, 0, 0);
			glXDestroyContext(_display, _tmpcontext);
			
			auto fbconfig = chooseFBConfig(_display, screen);
			
			if(!fbconfig) {
				throw Exception("Failed to get GLXFBConfig!");
			}
			
			XFree(_visualInfo);
			_visualInfo = glXGetVisualFromFBConfig(_display, fbconfig);
			
			if(!_visualInfo) {
				throw Exception("Failed to get visinfo!");
			}
			
			XDestroyWindow(_display, _wnd);
			
			{
				XSetWindowAttributes winAttr;
				
				winAttr.event_mask = StructureNotifyMask | KeyPressMask;
				winAttr.background_pixmap = 0;
				winAttr.background_pixel = 0;
				winAttr.border_pixel = 0;
				
				winAttr.colormap = XCreateColormap(_display, root, _visualInfo->visual, AllocNone);
				
				unsigned int mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;
				
				_wnd = XCreateWindow(_display, root, 0, 0, 100, 100, 0, _visualInfo->depth, InputOutput, _visualInfo->visual, mask, &winAttr);
			}
			
			if(!_wnd) {
				throw Exception("Cannot create X Window!");
			}
			
			static int attribs[] = {
				GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
				GLX_CONTEXT_MINOR_VERSION_ARB, 3,
				GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
#ifdef GL_DEBUG
				GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
#endif
				0
			};
			
			_context = glXCreateContextAttribsARB(_display, fbconfig, 0, True, attribs);
			XSync(_display, False);
			
			if(!_context) {
				throw Exception("Can't create OpenGL render context!");
			}
			
			if(glXMakeCurrent(_display, _wnd, _context) == GL_FALSE) {
				throw Exception("Can't bind OpenGL render context!");
			}
			
			if(!glXIsDirect(_display, glXGetCurrentContext())) {
				std::cout << "Indirect GLX rendering context obtained" << std::endl;
			}
#endif
			checkForErrors();
			
			glDisable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);
			glEnable(GL_SCISSOR_TEST);
			
			glFrontFace(GL_CW);
			
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			
#ifdef GL_DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			
			if(glDebugMessageCallback != nullptr) {
				glDebugMessageCallback(glDebugCallbackFunc, nullptr);
			} else {
				using namespace std;
				cout << "glDebugMessageCallback is not supported" << endl;
			}
#endif
			
			checkForErrors();
		}
		
		void GLGraphics::initFacilities() {
			initShaders();
			Graphics3D::initFacilities();
			checkForErrors();
		}
		
		void GLGraphics::bind(const GLShaderProgram * program) {
			if(_shaderProgram != program) {
				_shaderProgram = program;
				glUseProgram(program->id);
			}
		}
		
		void GLGraphics::bind(const Handle<Texture> & texture, uint index) {
			glActiveTexture(GL_TEXTURE0 + index);
			texture->apply();
		}
		
		void GLGraphics::printInfo() {}
		
		void GLGraphics::printDebug() {}
		
		void GLGraphics::checkForErrors() {
			GLenum error = glGetError();
			
			if(error != GL_NO_ERROR) {
				throw Exception("OpenGL error, code: ", String::hex(error));
			}
		}
		
		Handle<Image> GLGraphics::createImage(const ImageData & data) {
			return Handle<GLImage>(this, data);
		}
		
		Handle<VertexBuffer> GLGraphics::createVertexBuffer(VertexLayout * layout, const VertexData & data) {
			return Handle<GLVertexBuffer, GLGraphics>(this, layout, data);
		}
		
		Handle<MeshBuilder> GLGraphics::createMesh() {
			return Handle<GLMeshBuilder>(this);
		}
		
		Handle<UniformAdapter> & GLGraphics::init(Handle<UniformAdapter> & adapter, const char * name, ShaderType shader, int index, size_t size) {
			_uniformBindings.emplace_back(name, index);
			
			for(auto & p : _shaderPrograms) {
				auto id = cast::as<GLShaderProgram>(valueof(p))->id;
				auto block = glGetUniformBlockIndex(id, name);
				
				if(block != GL_INVALID_INDEX)
					glUniformBlockBinding(id, block, index);
			}
			
			checkForErrors();
			
			adapter = Handle<GLUniformAdapter, GLGraphics>(this, shader, index, static_cast<uint>(size));
			return adapter;
		}
		
		Handle<Surface> GLGraphics::createSurface(UISpace * space) {
			return Handle<UISurface>(this, space);
		}
		
		Handle<TextureSurface> GLGraphics::createSurface(const IntSize & size) {
			return Handle<GLTextureSurface>(this, size);
		}
		
		void GLGraphics::addShaderProgram(const string & id, VertexLayout * layout, ShaderCodeSet & codeSet) {
			_shaderPrograms[id] = handle<GLShaderProgram>(this, id, layout, codeSet);
		}
		
		void GLGraphics::present() const {
			_surface->present();
		}
		
		void GLGraphics::clip(const IntRect & rect) {
			if(_clipRect == rect) {
				return;
			}
			
			_clipRect = rect;
			glScissor(_clipRect.left, surface()->height() - _clipRect.bottom, _clipRect.width(), _clipRect.height());
		}
	}
}
