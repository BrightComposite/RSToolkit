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
		class GLVertexLayout;
		class GLShader;
		class GLShaderProgram;
		class GLMeshTrait;
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
			api(opengl3_3) void bind(const GLMeshTrait * mesh);

			virtual api(opengl3_3) void clip(const IntRect & rect) override;
			virtual api(opengl3_3) void present() const override;

			virtual api(opengl3_3) void printInfo();
			virtual api(opengl3_3) void printDebug();
			virtual api(opengl3_3) void checkForErrors() override;

			virtual api(opengl3_3) Handle<Image> createImage(const ImageData & data) override;
			virtual api(opengl3_3) Handle<Surface> createSurface(UISpace * space) override;
			virtual api(opengl3_3) Handle<Surface> createSurface(const IntSize & size, Handle<Image> & image) override;

			virtual api(opengl3_3) Handle<Mesh> createMesh(ArrayList<MeshBuffer> && buffers, uint verticesCount, VertexTopology topology, uint verticesLocation) override;
			virtual api(opengl3_3) Handle<Mesh> createMesh(ArrayList<MeshBuffer> && buffers, const VertexIndices & indices, uint verticesCount, VertexTopology topology, uint verticesLocation) override;
			virtual api(opengl3_3) Handle<InstancedMesh> createInstancedMesh(MeshInputLayout * instanceLayout, ArrayList<MeshBuffer> && buffers, uint verticesCount, VertexTopology topology, uint verticesLocation) override;
			virtual api(opengl3_3) Handle<InstancedMesh> createInstancedMesh(MeshInputLayout * instanceLayout, ArrayList<MeshBuffer> && buffers, const VertexIndices & indices, uint verticesCount, VertexTopology topology, uint verticesLocation) override;

			virtual api(opengl3_3) Handle<VertexBuffer> createVertexBuffer(VertexLayout * layout, const VertexData & data) override;

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

			virtual api(opengl3_3) Handle<VertexLayout> createVertexLayout(const string & fingerprint) override;

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
			const GLMeshTrait * _mesh = nullptr;
		};

		//---------------------------------------------------------------------------

		class GLImage : public Image
		{
			friend class TextureSurface;

		public:
			api(opengl3_3) GLImage(GLGraphics * graphics, uint width, uint height);
			api(opengl3_3) GLImage(GLGraphics * ctx, const ImageData & data);

			virtual api(opengl3_3) void apply() const override;
			virtual api(opengl3_3) void requestData(ImageData * output) const override;

		protected:
			float _scale[2];
			uint  _id;
			GLGraphics * _graphics;
		};

		//---------------------------------------------------------------------------

		class GLVertexLayout : public VertexLayout
		{
			friend_owned_handle(GLVertexLayout, GLGraphics);

		public:
			virtual ~GLVertexLayout() {}

			virtual api(opengl3_3) void apply();
			virtual api(opengl3_3) void accept(const ShaderCode * code);

			static const char * const attributes[];

		protected:
			api(opengl3_3) GLVertexLayout(GLGraphics * graphics, const string & fingerprint);

			GLGraphics * graphics;
		};

		//---------------------------------------------------------------------------

		class GLVertexBuffer : public VertexBuffer
		{
			friend_owned_handle(GLVertexBuffer, GLGraphics);

		public:
			virtual api(opengl3_3) ~GLVertexBuffer();

			virtual api(opengl3_3) void apply() const override;

		protected:
			api(opengl3_3) GLVertexBuffer(GLGraphics * graphics, VertexLayout * layout, const VertexData & vd);

			uint handle;
		};

		class GLIndexBuffer : public MeshBuffer
		{
			friend_owned_handle(GLIndexBuffer, GLGraphics);

		public:
			api(opengl3_3) GLIndexBuffer(GLGraphics * graphics, const VertexIndices & indices);
			virtual api(opengl3_3) ~GLIndexBuffer();

			virtual api(opengl3_3) void apply() const override;

		protected:
			uint handle;
		};

		//---------------------------------------------------------------------------

		class GLMeshTrait
		{
		public:
			api(opengl3_3) GLMeshTrait(GLGraphics * graphics, VertexTopology topology);
			virtual api(opengl3_3) ~GLMeshTrait();

		protected:
			uint id;
			uint topology;
			GLGraphics * graphics;
		};

		class GLIndexedMeshTrait : public GLMeshTrait
		{
		public:
			api(opengl3_3) GLIndexedMeshTrait(GLGraphics * graphics, VertexTopology topology, const VertexIndices & indices);

		protected:
			Handle<GLIndexBuffer> ibuffer;
		};

		class GLMesh : public Mesh, public GLMeshTrait
		{
		public:
			api(opengl3_3) GLMesh(GLGraphics * graphics, ArrayList<MeshBuffer> && buffers, VertexTopology topology, uint verticesCount, uint verticesLocation);

			virtual api(opengl3_3) void draw() const;

		protected:
			uint _state;
		};

		class GLIndexedMesh : public Mesh, public GLIndexedMeshTrait
		{
		public:
			api(opengl3_3) GLIndexedMesh(GLGraphics * graphics, ArrayList<MeshBuffer> && buffers, const VertexIndices & indices, VertexTopology topology, uint verticesCount, uint verticesLocation);

			virtual api(opengl3_3) void draw() const;

		protected:
		};

		class GLInstancedMeshBase : public InstancedMesh
		{
		public:
			api(opengl3_3) GLInstancedMeshBase(MeshInputLayout * instanceLayout, ArrayList<MeshBuffer> && buffers, uint verticesCount, uint verticesLocation);

			virtual api(opengl3_3) MeshInstance * createInstance() override;
			virtual api(opengl3_3) void setData(uint index, void * data) override;

		protected:
		};

		class GLInstancedMesh : public GLInstancedMeshBase, public GLMeshTrait
		{
		public:
			api(opengl3_3) GLInstancedMesh(GLGraphics * graphics, MeshInputLayout * instanceLayout, ArrayList<MeshBuffer> && buffers, VertexTopology topology, uint verticesCount, uint verticesLocation);

			virtual api(opengl3_3) void draw() const;

		protected:
		};

		class GLInstancedIndexedMesh : public GLInstancedMeshBase, public GLIndexedMeshTrait
		{
		public:
			api(opengl3_3) GLInstancedIndexedMesh(GLGraphics * graphics, MeshInputLayout * instanceLayout, ArrayList<MeshBuffer> && buffers, const VertexIndices & indices, VertexTopology topology, uint verticesCount, uint verticesLocation);
	
			virtual api(opengl3_3) void draw() const;

		protected:
		};

		//---------------------------------------------------------------------------

		struct GLUniformBuffer
		{
			owned_data<byte> data;
			uint handle;
		};

		class GLUniformAdapter : public UniformAdapter
		{
			friend_owned_handle(GLUniformAdapter, GLGraphics);

		public:
			virtual api(opengl3_3) ~GLUniformAdapter();

			virtual api(opengl3_3) void update(const void * data) override;

			virtual api(opengl3_3) void update() override;
			virtual api(opengl3_3) void append(UniformData & data) override;
			virtual api(opengl3_3) void bind(const UniformData & data) override;

		protected:
			api(opengl3_3) GLUniformAdapter(GLGraphics * graphics, ShaderType shader, int index, uint size);

			uint _common;
			size_t _offset;
			array_list<GLUniformBuffer> _buffers;
			GLGraphics * _graphics;
		};

		//---------------------------------------------------------------------------

		inline GLenum glShaderType(ShaderType type)
		{
			switch(type)
			{
				case ShaderType::Vertex:
					return GL_VERTEX_SHADER;
				case ShaderType::Fragment:
					return GL_FRAGMENT_SHADER;
				default:
					return 0;
			}
		}

		class GLShaderProgram : public ShaderProgram
		{
		public:
			api(opengl3_3) GLShaderProgram(GLGraphics * graphics, const string & filename, VertexLayout * layout, ShaderCodeState state = ShaderCodeState::Raw);
			api(opengl3_3) GLShaderProgram(GLGraphics * graphics, const string & key, VertexLayout * layout, const ShaderCodeSet & codeSet, ShaderCodeState state = ShaderCodeState::Raw);
			api(opengl3_3) ~GLShaderProgram();

			virtual void apply() const override;

			GLGraphics * graphics;
			VertexLayout * layout;
			GLuint id = 0;

		protected:
			api(opengl3_3) GLShaderProgram(GLGraphics * graphics, VertexLayout * layout);

			Map<ShaderType, GLShader> _shaders;
		};

		class GLShader : public Shader
		{
			friend class GLShaderProgram;

		public:
			api(opengl3_3) GLShader(GLShaderProgram * program, const String & path, ShaderCodeState state, GLenum type);
			api(opengl3_3) GLShader(GLShaderProgram * program, const Handle<ShaderCode> & code, ShaderCodeState state, GLenum type);
			virtual api(opengl3_3) ~GLShader();

		protected:
			api(opengl3_3) void read(const String & filename, GLenum type);
			api(opengl3_3) void compile(const String & filename, GLenum type);
			api(opengl3_3) void initRaw(const Handle<ShaderCode> & code, GLenum type);
			api(opengl3_3) void initCompiled(const Handle<ShaderCode> & code, GLenum type);
		
			GLShaderProgram * program;
			GLuint id;
		};

		//---------------------------------------------------------------------------

		class GLSurface : public Surface
		{
			friend class GLGraphics;

		public:
			api(opengl3_3) GLSurface(GLGraphics * graphics, const IntSize & size);
			virtual api(opengl3_3) ~GLSurface();

			virtual api(opengl3_3) void clear() const override;
			virtual api(opengl3_3) void present() const override;

		protected:
			api(opengl3_3) void createDepthStencil();

			GLGraphics * _graphics;
			HDC _deviceCtx = nullptr;
			Context _renderCtx = nullptr;
		};

		class RenderTargetSurface : public GLSurface
		{
			deny_copy(RenderTargetSurface);

		public:
			api(opengl3_3) RenderTargetSurface(GLGraphics * graphics, const IntSize & size);
			virtual api(opengl3_3) ~RenderTargetSurface();

			virtual api(opengl3_3) void apply() const override;

		protected:
		};

		class DepthSurface : public GLSurface
		{
			deny_copy(DepthSurface);

		public:
			api(opengl3_3) DepthSurface(GLGraphics * graphics, const IntSize & size);
			virtual ~DepthSurface() {}

			virtual api(opengl3_3) void apply() const override;
			virtual api(opengl3_3) void clear() const override;
		};

		class UISurface : public RenderTargetSurface, public Connector
		{
			deny_copy(UISurface);

		public:
			api(opengl3_3) UISurface(GLGraphics * graphics, UISpace * space);
			virtual ~UISurface() {}

			virtual api(opengl3_3) void present() const override;
			virtual api(opengl3_3) void requestData(ImageData * data) const override;

			virtual api(opengl3_3) void resize() override;

		private:
			void createRenderTarget();
			void releaseRenderTarget();

			void onUIResize(Handle<UIResizeMessage> & msg, UISpace & space);
			void onUIFullscreen(Handle<UIFullscreenMessage> & msg, UISpace & space);
			void onUIDestroy(Handle<UIDestroyMessage> & msg, UISpace & space);

			UISpace * _space;
		};

		class TextureSurface : public RenderTargetSurface
		{
			friend class GLGraphics;

		public:
			api(opengl3_3) TextureSurface(GLGraphics * graphics, const IntSize & size, Handle<Image> & image);
			virtual ~TextureSurface() {}

			virtual api(opengl3_3) void clear() const override;
			virtual api(opengl3_3) void requestData(ImageData * data) const override;

		protected:
			Handle<GLImage> _texture;
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
