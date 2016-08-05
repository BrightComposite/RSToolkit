//---------------------------------------------------------------------------

#pragma once

#ifndef GL_OBJECTS_H
#define GL_OBJECTS_H

//---------------------------------------------------------------------------

#include "OpenGL3_3.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace OpenGL
	{
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

		struct GLMeshImpl
		{
			virtual ~GLMeshImpl() {}
			virtual void draw(const GLMesh * mesh) const = 0;

			template<class T>
			T * upcast()
			{
				return dynamic_cast<T *>(this);
			}

			template<class T>
			const T * upcast() const
			{
				return dynamic_cast<const T *>(this);
			}
		};

		class GLMesh : public Mesh
		{
		public:
			api(opengl3_3) GLMesh(GLGraphics * graphics);
			virtual api(opengl3_3) ~GLMesh();

			virtual api(opengl3_3) Mesh * buffer(const Handle<VertexBuffer> & buffer) final;
			virtual api(opengl3_3) Mesh * indices(const VertexIndices & indices, uint offset) final;
			virtual api(opengl3_3) Mesh * topology(VertexTopology t) final;
			virtual api(opengl3_3) Mesh * instanceLayout(VertexLayout *) final;

			virtual api(opengl3_3) const Mesh * ready() final;

			virtual api(opengl3_3) MeshInstance * instance() const final;

			virtual api(opengl3_3) void draw() const final;

			uint topology() const
			{
				return _topology;
			}

		protected:
			GLGraphics * _graphics;
			uint _id;
			uint _topology;
			Unique<GLMeshImpl> _impl;
			ArrayList<VertexBuffer> _buffers;
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
}

//---------------------------------------------------------------------------
#endif
