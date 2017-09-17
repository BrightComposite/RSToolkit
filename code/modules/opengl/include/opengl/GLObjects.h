//---------------------------------------------------------------------------

#pragma once

#ifndef GL_OBJECTS_H
#define GL_OBJECTS_H

//---------------------------------------------------------------------------

#include "opengl.h"

//---------------------------------------------------------------------------

namespace asd
{
	uniform_class
	(
		opengl,
		ImageScale, 1, Vertex,
		(float2, scale)
	);

	namespace opengl
	{
		class GLImage : public image
		{
			friend class GLTextureSurface;

		public:
			api(opengl) GLImage(GLGraphics * graphics, uint width, uint height, image_format format);
			api(opengl) GLImage(GLGraphics * ctx, const image_data & data);

			virtual api(opengl) ~GLImage();

			api(opengl) void setSize(const int_size & size);

			virtual api(opengl) void apply() const override;
			virtual api(opengl) void requestData(image_data * output) const override;

		protected:
			float2 _scale;
			uint  _id;
			GLenum _target;
			GLGraphics * _graphics;
		};

		//---------------------------------------------------------------------------

		class GLVertexBuffer : public vertex_buffer
		{
			friend_owned_handle(GLVertexBuffer)
			
		public:
			virtual api(opengl) ~GLVertexBuffer();

			virtual api(opengl) void apply() const override;

		protected:
			api(opengl) GLVertexBuffer(GLGraphics * graphics, vertex_layout * layout, const vertex_data & vd);

			uint handle;
		};

		class GLIndexBuffer : public mesh_buffer
		{
		public:
			api(opengl) GLIndexBuffer(GLGraphics * graphics, const vertex_indices & indices);
			virtual api(opengl) ~GLIndexBuffer();

			virtual api(opengl) void apply() const override;

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

		class mesh_builder : public mesh_builder
		{
			template<GLMeshType type>
			friend class GLMesh;

		public:
			api(opengl) GLMeshBuilder( * graphics);
			virtual api(opengl) ~GLMeshBuilder();

			virtual api(opengl) mesh_builder * buffer(const handle<vertex_buffer> & buffer) override final;
			virtual api(opengl) mesh_builder * indices(const vertex_indices & indices) override final;
			virtual api(opengl) mesh_builder * offset(uint offset) override final;
			virtual api(opengl) mesh_builder * topology(vertex_topology t) override final;
			virtual api(opengl) mesh_builder * makeInstanced(vertex_layout *) override final;

			virtual api(opengl) handle<mesh> ready() override final;

		protected:
			GLGraphics * _graphics;
			uint _topology = 0;
			uint _offset = 0;
			uint _verticesCount = 0;
			uint _indicesCount = 0;
			array_list<vertex_buffer> _buffers;
			handle<GLIndexBuffer> _ibuffer = nullptr;
			vertex_layout * _instancedLayout = nullptr;
			unique<instanced_mesh_data> _instancedData = nullptr;
		};

		//---------------------------------------------------------------------------

		struct GLUniformBuffer
		{
			owned_data<byte> data;
			uint handle;
		};

		class GLUniformAdapter : public UniformAdapter
		{
			friend_owned_handle(GLUniformAdapter)
		
		public:
			virtual api(opengl) ~GLUniformAdapter();

			virtual api(opengl) void update(const void * data) override;

			virtual api(opengl) void update() override;
			virtual api(opengl) void append(UniformData & data) override;
			virtual api(opengl) void bind(const UniformData & data) override;

		protected:
			api(opengl) GLUniformAdapter(GLGraphics * graphics, ShaderType shader, int index, uint size);
			
			GLGraphics * _graphics;
			uint _common;
			size_t _offset;
			array_list<GLUniformBuffer> _buffers;
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
			api(opengl) GLShaderProgram(GLGraphics * graphics, const string & filename, vertex_layout * layout, ShaderCodeState state = ShaderCodeState::Raw);
			api(opengl) GLShaderProgram(GLGraphics * graphics, const string & key, vertex_layout * layout, const ShaderCodeSet & codeSet, ShaderCodeState state = ShaderCodeState::Raw);
			api(opengl) ~GLShaderProgram();

			virtual void apply() const override;

			GLGraphics * graphics;
			GLuint id = 0;
			vertex_layout * layout;

		protected:
			api(opengl) GLShaderProgram(GLGraphics * graphics, vertex_layout * layout);

			Dictionary<ShaderType, GLShader> _shaders;
		};

		class GLShader : public Shader
		{
			friend class GLShaderProgram;

		public:
			api(opengl) GLShader(GLShaderProgram * program, const String & path, ShaderCodeState state, GLenum type);
			api(opengl) GLShader(GLShaderProgram * program, const handle<ShaderCode> & code, ShaderCodeState state, GLenum type);
			virtual api(opengl) ~GLShader();

		protected:
			api(opengl) void read(const String & filename, GLenum type);
			api(opengl) void compile(const String & filename, GLenum type);
			api(opengl) void initRaw(const handle<ShaderCode> & code, GLenum type);
			api(opengl) void initCompiled(const handle<ShaderCode> & code, GLenum type);

			GLShaderProgram * program;
			GLuint id;
		};

		//---------------------------------------------------------------------------

		class GLSurface : public surface
		{
			friend class GLGraphics;

		public:
			api(opengl) GLSurface(GLGraphics * graphics, const int_size & size);
			virtual api(opengl) ~GLSurface();

			virtual api(opengl) void clear() const override;
			virtual api(opengl) void present() const override;

		protected:
			api(opengl) void createDepthStencil();

			GLGraphics * _graphics;
#if BOOST_OS_WINDOWS
			DeviceContext _deviceCtx = nullptr;
#endif
		};

		class DepthSurface : public GLSurface
		{
			deny_copy(DepthSurface);

		public:
			api(opengl) DepthSurface(GLGraphics * graphics, const int_size & size);
			virtual ~DepthSurface() {}

			virtual api(opengl) void apply() const override;
			virtual api(opengl) void clear() const override;
		};

		class UISurface : public GLSurface, public Connector
		{
			deny_copy(UISurface);

		public:
			api(opengl) UISurface(GLGraphics * graphics, ui_space * space);
			virtual ~UISurface() {}

			virtual api(opengl) void apply() const override;

			virtual api(opengl) void present() const override;
			virtual api(opengl) void requestData(image_data * data) const override;

			virtual api(opengl) void resize() override;

		private:
			void createRenderTarget();
			void releaseRenderTarget();

			void onUIResize(handle<UIResizeMessage> & msg, ui_space & space);
			void onUIFullscreen(handle<UIFullscreenMessage> & msg, ui_space & space);
			void onUIDestroy(handle<UIDestroyMessage> & msg, ui_space & space);

			ui_space * _space;
		};

		class GLTextureSurface : public TextureSurface
		{
			friend class GLGraphics;

		public:
			api(opengl) GLTextureSurface(GLGraphics * graphics, const int_size & size);
			virtual api(opengl) ~GLTextureSurface();

			virtual api(opengl) void apply() const override;
			virtual api(opengl) void clear() const override;
			virtual api(opengl) void clear(uint bitmask) const override;

			virtual api(opengl) void present() const override;

			virtual api(opengl) void requestData(image_data * data) const override;

			virtual api(opengl) void addBuffer(handle<Texture> & texture, image_format format = image_format::rgb) override;

		protected:
			virtual api(opengl) void resize() override;

			GLGraphics * _graphics;
			array_list<GLImage> _textures;
			GLuint id;
			GLuint depthBuffer;
			array_list<GLenum> buffers;
		};
	}
}

//---------------------------------------------------------------------------
#endif
