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
			Handle<const Mesh, Graphics3D> quad;
			Handle<const Mesh, Graphics3D> texquad;
			Handle<const Mesh, Graphics3D> linequad;
		};

		struct Meshes3D
		{
			Handle<const Mesh, Graphics3D> quad;
			Handle<const Mesh, Graphics3D> texquad;
			Handle<const Mesh, Graphics3D> linequad;
			Handle<const Mesh, Graphics3D> cube;
			Handle<const Mesh, Graphics3D> texcube;
			Handle<const Mesh, Graphics3D> colorcube;
			Handle<const Mesh, Graphics3D> linecube;
		};

	public:
		api(graphics) Graphics3D();
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
		api(graphics) VertexElement * getVertexElement(const string & key);

		api(graphics) void registerVertexElement(const string & key, const string & semantic, uint units);

		template<size_t N>
		void addShaderPrograms(const RawShaderCode (&codeSets)[N])
		{
			addShaderPrograms(codeSets, N);
		}

		api(graphics) void addShaderPrograms(const RawShaderCode * codeSets, size_t count);
		api(graphics) const Handle<ShaderProgram> & getShaderProgram(const string & id);

		virtual void addShaderProgram(const string & id, VertexLayout * layout, ShaderCodeSet & codeSet) = 0;

		virtual Handle<VertexBuffer> createVertexBuffer(VertexLayout * layout, const VertexData & data) = 0;
		virtual Handle<Mesh> createMesh() = 0;

		Handle<const Mesh> createMesh(const Handle<VertexBuffer> & buffer, VertexTopology topology = VertexTopology::Triangles)
		{
			return createMesh()->buffer(buffer)->topology(topology)->ready();
		}

		Handle<const Mesh> createMesh(const Handle<VertexBuffer> & buffer, const VertexIndices & indices, VertexTopology topology = VertexTopology::Triangles)
		{
			return createMesh()->buffer(buffer)->indices(indices)->topology(topology)->ready();
		}

		Handle<const Mesh> createMesh(VertexLayout * layout, const VertexData & data, VertexTopology topology = VertexTopology::Triangles)
		{
			return createMesh(createVertexBuffer(layout, data), topology);
		}

		Handle<const Mesh> createMesh(VertexLayout * layout, const VertexData & data, const VertexIndices & indices, VertexTopology topology = VertexTopology::Triangles)
		{
			return createMesh(createVertexBuffer(layout, data), indices, topology);
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
			uniformAdapter<T>()->update<T>(forward<A>(args)...);
		}

		template<class T, useif<is_uniform<T>::value>>
		void updateUniform(const Contents<T> & contents)
		{
			uniformAdapter<T>()->update<T>(contents);
		}

		template<class T>
		void appendUniformData(UniformData & data)
		{
			return uniformAdapter<T>()->append(data);
		}

		void updateUniformBuffers()
		{
			for(auto & a : _uniforms)
				valueof(a)->update();
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

		virtual Handle<UniformAdapter> & init(Handle<UniformAdapter> & adapter, const char * name, ShaderType shader, int index, size_t size) = 0;
		
		template<class T>
		UniformAdapter * uniformAdapter()
		{
			auto & a = _uniforms[morphid(T)];
			return a != nullptr ? a : init(a, T::name(), T::shader, T::index, sizeof(Contents<T>));
		}

		float _depth = 0.0f;
		StateHandle<bool> _depthTestMode {false};
		StateHandle<bool> _blendMode {false};

		UniformSet _uniforms;

		ArrayList<Texture> _textures;

		Map<string, VertexElement, Graphics3D> _vertexElements;
		UnorderedMap<string, VertexLayout, Graphics3D> _vertexLayouts;
		UnorderedMap<string, ShaderProgram> _shaderPrograms;
	};
}

//---------------------------------------------------------------------------
#endif
