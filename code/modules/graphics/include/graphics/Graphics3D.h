//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHICS_3D_H
#define GRAPHICS_3D_H

//---------------------------------------------------------------------------

#include "Graphics.h"
#include "GraphicModel.h"

//---------------------------------------------------------------------------

namespace asd
{
	class Graphics3D;

	link_class(graphics, Graphics3D, Class<Graphics>);

	using Texture = Image;
	using TextureSurface = ImageSurface;

	class Graphics3D : public Graphics
	{
		deny_copy(Graphics3D);

		friend class Mesh;
		friend class IndexedMesh;

	public:
		api(graphics) Graphics3D(const Display & d);
		virtual ~Graphics3D() {}

		using Graphics::bind;
		using Graphics::draw;

		virtual api(graphics) void bind(const Handle<Texture> & texture, uint index);

		virtual api(graphics) void rectangle(const IntRect & rect) override final;
		virtual api(graphics) void ellipse(const IntRect & rect) override final;
		virtual api(graphics) void rectangle(const SqRect & rect) override final;
		virtual api(graphics) void ellipse(const SqRect & rect) override final;

		api(graphics) void setArea(const IntRect & rect);
		api(graphics) void setArea(const SqRect & rect);

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
		virtual Handle<MeshBuilder> createMesh() = 0;

		Handle<Mesh> createMesh(const Handle<VertexBuffer> & buffer, VertexTopology topology = VertexTopology::Triangles)
		{
			return createMesh()->buffer(buffer)->topology(topology)->ready();
		}

		Handle<Mesh> createMesh(const Handle<VertexBuffer> & buffer, const VertexIndices & indices, VertexTopology topology = VertexTopology::Triangles)
		{
			return createMesh()->buffer(buffer)->indices(indices)->topology(topology)->ready();
		}

		Handle<Mesh> createMesh(VertexLayout * layout, const VertexData & data, VertexTopology topology = VertexTopology::Triangles)
		{
			return createMesh(createVertexBuffer(layout, data), topology);
		}

		Handle<Mesh> createMesh(VertexLayout * layout, const VertexData & data, const VertexIndices & indices, VertexTopology topology = VertexTopology::Triangles)
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

		bool accumulationMode() const
		{
			return *_accumulationMode;
		}

		void setDepthTestMode(bool mode)
		{
			*_depthTestMode = mode;
		}

		void setBlendMode(bool mode)
		{
			*_blendMode = mode;
		}

		void setAccumulationMode(bool mode)
		{
			*_accumulationMode = mode;
		}

		State<bool> * depthTestState()
		{
			return _depthTestMode;
		}

		State<bool> * blendState()
		{
			return _blendMode;
		}

		State<bool> * accumulationState()
		{
			return _accumulationMode;
		}

		template<class U, typename ... A, useif<is_uniform<U>::value, can_construct_contents<U, A...>::value>>
		void updateUniform(A && ... args)
		{
			uniformAdapter<U>()->template update<U>(forward<A>(args)...);
		}

		template<class U, useif<is_uniform<U>::value>>
		void updateUniform(const Contents<U> & contents)
		{
			uniformAdapter<U>()->template update<U>(contents);
		}

		void updateUniformBuffers()
		{
			for(auto & a : _uniforms)
				valueof(a)->update();
		}

		struct
		{
			Handle<FxTechnique, Graphics3D> rectangle;
			Handle<FxTechnique, Graphics3D> ellipse;
			Handle<FxTechnique, Graphics3D> wired_rectangle;
			Handle<FxTechnique, Graphics3D> wired_ellipse;
			Handle<FxTechnique, Graphics3D> figure;
			Handle<FxTechnique, Graphics3D> image;
			Handle<FxTechnique, Graphics3D> text;
		} techniques2d;

		struct
		{
			Handle<FxTechnique, Graphics3D> color;
			Handle<FxTechnique, Graphics3D> multicolor;
			Handle<FxTechnique, Graphics3D> texture;
		} techniques3d;

		struct
		{
			Handle<const Mesh, Graphics3D> quad;
			Handle<const Mesh, Graphics3D> texquad;
			Handle<const Mesh, Graphics3D> linequad;
		} meshes2d;

		struct
		{
			Handle<const Mesh, Graphics3D> quad;
			Handle<const Mesh, Graphics3D> texquad;
			Handle<const Mesh, Graphics3D> linequad;

			Handle<const Mesh, Graphics3D> cube;
			Handle<const Mesh, Graphics3D> texcube;
			Handle<const Mesh, Graphics3D> colorcube;
			Handle<const Mesh, Graphics3D> normalcube;
			Handle<const Mesh, Graphics3D> normaltexcube;
			Handle<const Mesh, Graphics3D> normalcolorcube;
			Handle<const Mesh, Graphics3D> linecube;
		} meshes3d;

	protected:
		friend UniformSet;
		template<class U>
		friend struct UniformChunk;

		virtual api(graphics) void initFacilities() override;
		virtual api(graphics) void updateBrushState() override;

		virtual api(graphics) void updateSurface() override;

		api(graphics) void clearFacilities();

		virtual Handle<UniformAdapter> & init(Handle<UniformAdapter> & adapter, const char * name, ShaderType shader, int index, size_t size) = 0;
		
		template<class U>
		UniformAdapter * uniformAdapter()
		{
			auto & a = _uniforms[morphid(U)];
			return a != nullptr ? a : init(a, U::name(), U::shader, U::index, sizeof(Contents<U>));
		}

		template<class U>
		inline void uniformChunk(UniformChunk<U> & chunk);

		float _depth = 0.0f;
		StateHandle<bool> _depthTestMode {false};
		StateHandle<bool> _blendMode {false};
		StateHandle<bool> _accumulationMode {false};

		UniformSet _uniforms;

		ArrayList<Texture> _textures;

		Dictionary<string, VertexElement, Graphics3D> _vertexElements;
		Map<string, VertexLayout, Graphics3D> _vertexLayouts;
		Map<string, ShaderProgram> _shaderPrograms;
	};
	
	template<class U>
	struct UniformChunk : UniformData
	{
		static_assert(is_uniform<U>::value, "U must be an uniform");
		
		UniformChunk(Graphics3D * graphics) {
			graphics->uniformChunk(*this);
		}
		
		void fill(const Contents<U> & contents) {
			Memory<void>::move(bytes.ptr, contents.pointer(), bytes.size);
		}
	};
	
	template<class U>
	inline void Graphics3D::uniformChunk(UniformChunk<U> & chunk)
	{
		uniformAdapter<U>()->append(chunk);
	}
}

//---------------------------------------------------------------------------
#endif
