//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHICS_3D_H
#define GRAPHICS_3D_H

//---------------------------------------------------------------------------

#include "Graphics.h"
#include "GraphicModel.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;

	link_class(graphics, Graphics3D, Class<Graphics>);

	using Texture = Image;

	class Graphics3D : public Graphics
	{
		deny_copy(Graphics3D);

		friend class Mesh;
		friend class IndexedMesh;

		struct Techniques2D
		{
			Handle<FxTechnique, Graphics3D> rectangle;
			Handle<FxTechnique, Graphics3D> ellipse;
			Handle<FxTechnique, Graphics3D> wired_rectangle;
			Handle<FxTechnique, Graphics3D> wired_ellipse;
			Handle<FxTechnique, Graphics3D> figure;
			Handle<FxTechnique, Graphics3D> image;
			Handle<FxTechnique, Graphics3D> text;
		};

		struct Techniques3D
		{
			Handle<FxTechnique, Graphics3D> color;
			Handle<FxTechnique, Graphics3D> multicolor;
			Handle<FxTechnique, Graphics3D> texture;
		};

		struct Meshes2D
		{
			Handle<Mesh, Graphics3D> quad;
			Handle<Mesh, Graphics3D> texquad;
			Handle<Mesh, Graphics3D> linequad;
		};

		struct Meshes3D
		{
			Handle<Mesh, Graphics3D> quad;
			Handle<Mesh, Graphics3D> texquad;
			Handle<Mesh, Graphics3D> linequad;
			Handle<Mesh, Graphics3D> cube;
			Handle<Mesh, Graphics3D> texcube;
			Handle<Mesh, Graphics3D> colorcube;
			Handle<Mesh, Graphics3D> linecube;
		};

	public:
		Graphics3D() { setclass(Graphics3D); }
		virtual ~Graphics3D() {}

		using Graphics::bind;
		using Graphics::draw;

		api(graphics) void bind(const Handle<Texture> & texture, uint index);

		virtual api(graphics) void rectangle(const IntRect & rect) override final;
		virtual api(graphics) void ellipse(const IntRect & rect) override final;
		virtual api(graphics) void rectangle(const SqRect & rect) override final;
		virtual api(graphics) void ellipse(const SqRect & rect) override final;

		virtual api(graphics) void draw(const Figure * figure, const IntRect & bounds) override final;
		virtual api(graphics) void draw(const Figure * figure, const FloatTransform & transform) override final;
		virtual api(graphics) void draw(const Image * image, const IntRect & rect) override final;
		virtual api(graphics) void draw(const Image * image, const SqRect & rect) override final;
		virtual api(graphics) void draw(const Symbol * symbol, int x, int y) override final;

		api(graphics) VertexLayout * getVertexLayout(const string & fingerprint);
		api(graphics) const Handle<ShaderProgram> & getShaderProgram(const string & id);

		virtual Handle<Mesh> createMesh(const Handle<VertexBuffer> & buffer, VertexTopology topology = VertexTopology::Triangles, uint verticesLocation = 0) = 0;
		virtual Handle<IndexedMesh> createMesh(const Handle<VertexBuffer> & buffer, const VertexIndices & indices, VertexTopology topology = VertexTopology::Triangles, uint verticesLocation = 0, uint indicesLocation = 0) = 0;

		Handle<Mesh> Graphics3D::createMesh(VertexLayout * layout, const VertexData & data, VertexTopology topology = VertexTopology::Triangles, uint verticesLocation = 0)
		{
			return createMesh(createVertexBuffer(layout, data), topology, verticesLocation);
		}

		Handle<IndexedMesh> Graphics3D::createMesh(VertexLayout * layout, const VertexData & data, const VertexIndices & indices, VertexTopology topology = VertexTopology::Triangles, uint verticesLocation = 0, uint indicesLocation = 0)
		{
			return createMesh(createVertexBuffer(layout, data), indices, topology, verticesLocation, indicesLocation);
		}

		Handle<Mesh> Graphics3D::createMesh(const string & fingerprint, const VertexData & data, VertexTopology topology = VertexTopology::Triangles, uint verticesLocation = 0)
		{
			return createMesh(getVertexLayout(fingerprint), data, topology, verticesLocation);
		}

		Handle<IndexedMesh> Graphics3D::createMesh(const string & fingerprint, const VertexData & data, const VertexIndices & indices, VertexTopology topology = VertexTopology::Triangles, uint verticesLocation = 0, uint indicesLocation = 0)
		{
			return createMesh(getVertexLayout(fingerprint), data, indices, topology, verticesLocation, indicesLocation);
		}

		float depth()
		{
			return _depth;
		}

		void setDepth(float depth)
		{
			_depth = depth;
		}

		bool depthTestMode() const
		{
			return *_depthTestMode;
		}

		bool blendMode() const
		{
			return *_blendMode;
		}

		void setAlphaTestMode(bool mode)
		{
			*_blendMode = mode;
		}

		void setDepthTestMode(bool mode)
		{
			*_depthTestMode = mode;
		}

		State<bool> * depthTestState()
		{
			return _depthTestMode;
		}

		State<bool> * blendState()
		{
			return _blendMode;
		}

		template<class T, typename ... A, useif<is_uniform<T>::value, can_construct_contents<T, A...>::value>>
		void updateUniform(A && ... args)
		{
			_uniforms.require<T>(this)->set(forward<A>(args)...);
		}

		template<class T, useif<is_uniform<T>::value>>
		void updateUniform(const Contents<T> & contents)
		{
			_uniforms.require<T>(this)->set(contents);
		}

		Techniques2D techniques2d;
		Techniques3D techniques3d;
		Meshes2D meshes2d;
		Meshes3D meshes3d;

	protected:
		friend UniformSet;

		virtual api(graphics) void initFacilities() override;
		virtual api(graphics) void updateBrushState() override;

		virtual api(graphics) void updateSurface() override;

		api(graphics) void updateAreaUniform(const IntRect & rect);
		api(graphics) void updateAreaUniform(const SqRect & rect);
		
		api(graphics) void clearFacilities();

		virtual Handle<VertexLayout> createVertexLayout(const string & fingerprint) = 0;
		virtual Handle<VertexBuffer> createVertexBuffer(VertexLayout * layout, const VertexData & data) = 0;
		virtual Handle<IndexBuffer> createIndexBuffer(const VertexIndices & indices) = 0;

		virtual UniqueHandle<UniformAdapter> createUniformAdapter(const char * name, ShaderType shader, int index, size_t size) = 0;

		template<class T, useif<is_uniform<T>::value>>
		Handle<T, Graphics3D> & init(Handle<T, Graphics3D> & uniform)
		{
			return uniform.init(createUniformAdapter(T::name(), T::shader, T::index, sizeof(Contents<T>)));
		}

		float _depth = 1.0f;
		StateHandle<bool> _depthTestMode {false};
		StateHandle<bool> _blendMode {false};

		UniformSet _uniforms;

		ArrayList<Texture> _textures;

		UnorderedMap<string, VertexLayout> _vertexLayouts;
		UnorderedMap<string, ShaderProgram> _shaderPrograms;
	};
}

//---------------------------------------------------------------------------
#endif
