//---------------------------------------------------------------------------

#pragma once

#ifndef GL_OBJECTS_H
#define GL_OBJECTS_H

//---------------------------------------------------------------------------

#include "OpenGL3_3.h"

//---------------------------------------------------------------------------

namespace asd
{
	uniform_class
	(
		opengl3_3,
		ImageScale, 1, Vertex,
		(float2, scale)
	);

	namespace OpenGL
	{
		class GLImage : public Image
		{
			friend class GLTextureSurface;

		public:
			api(opengl3_3) GLImage(GLGraphics * graphics, uint width, uint height, ImageFormat format);
			api(opengl3_3) GLImage(GLGraphics * ctx, const ImageData & data);

			virtual api(opengl3_3) ~GLImage();

			api(opengl3_3) void setSize(const IntSize & size);

			virtual api(opengl3_3) void apply() const override;
			virtual api(opengl3_3) void requestData(ImageData * output) const override;

		protected:
			float2 _scale;
			uint  _id;
			GLenum _target;
			GLGraphics * _graphics;
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

		enum class GLMeshType
		{
			Plain,
			Indexed,
			Instanced,
			InstancedIndexed
		};

		template<GLMeshType type>
		class GLMesh {};

		class GLMeshBuilder : public MeshBuilder
		{
			template<GLMeshType type>
			friend class GLMesh;

		public:
			api(opengl3_3) GLMeshBuilder(GLGraphics * graphics);
			virtual api(opengl3_3) ~GLMeshBuilder();

			virtual api(opengl3_3) MeshBuilder * buffer(const Handle<VertexBuffer> & buffer) override final;
			virtual api(opengl3_3) MeshBuilder * indices(const VertexIndices & indices) override final;
			virtual api(opengl3_3) MeshBuilder * offset(uint offset) override final;
			virtual api(opengl3_3) MeshBuilder * topology(VertexTopology t) override final;
			virtual api(opengl3_3) MeshBuilder * makeInstanced(VertexLayout *) override final;

			virtual api(opengl3_3) Handle<Mesh> ready() override final;

		protected:
			GLGraphics * _graphics;
			uint _topology = 0;
			uint _offset = 0;
			uint _verticesCount = 0;
			uint _indicesCount = 0;
			ArrayList<VertexBuffer> _buffers;
			Handle<GLIndexBuffer> _ibuffer = nullptr;
			VertexLayout * _instancedLayout = nullptr;
			Unique<InstancedMeshData> _instancedData = nullptr;
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

			Dictionary<ShaderType, GLShader> _shaders;
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

		class UISurface : public GLSurface, public Connector
		{
			deny_copy(UISurface);

		public:
			api(opengl3_3) UISurface(GLGraphics * graphics, UISpace * space);
			virtual ~UISurface() {}

			virtual api(opengl3_3) void apply() const override;

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

		class GLTextureSurface : public TextureSurface
		{
			friend class GLGraphics;

		public:
			api(opengl3_3) GLTextureSurface(GLGraphics * graphics, const IntSize & size);
			virtual api(opengl3_3) ~GLTextureSurface();

			virtual api(opengl3_3) void apply() const override;
			virtual api(opengl3_3) void clear() const override;
			virtual api(opengl3_3) void clear(uint bitmask) const override;

			virtual api(opengl3_3) void present() const override;

			virtual api(opengl3_3) void requestData(ImageData * data) const override;

			virtual api(opengl3_3) void addBuffer(Handle<Texture> & texture, ImageFormat format = ImageFormat::rgb) override;

		protected:
			virtual api(opengl3_3) void resize() override;

			GLGraphics * _graphics;
			ArrayList<GLImage> _textures;
			GLuint id;
			GLuint depthBuffer;
			array_list<GLenum> buffers;
		};
	}
}

//---------------------------------------------------------------------------
#endif
