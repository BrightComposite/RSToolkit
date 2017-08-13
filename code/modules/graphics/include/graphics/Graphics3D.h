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
	
	using Texture = image;
	using TextureSurface = ImageSurface;

	class Graphics3D : public graphics
	{
		deny_copy(Graphics3D);

		friend class mesh;
		friend class IndexedMesh;

	public:
		api(graphics) Graphics3D();
		virtual ~Graphics3D() {}

		using graphics::bind;
		using graphics::draw;

		virtual api(graphics) void bind(const handle<Texture> & texture, uint index);

		virtual api(graphics) void rectangle(const math::int_rect & rect) override final;
		virtual api(graphics) void ellipse(const math::int_rect & rect) override final;
		virtual api(graphics) void rectangle(const sq_rect & rect) override final;
		virtual api(graphics) void ellipse(const sq_rect & rect) override final;

		api(graphics) void setArea(const math::int_rect & rect);
		api(graphics) void setArea(const sq_rect & rect);

		virtual api(graphics) void draw(const Figure * figure, const math::int_rect & bounds) override final;
		virtual api(graphics) void draw(const Figure * figure, const math::float_transform & transform) override final;
		virtual api(graphics) void draw(const image * image, const math::int_rect & rect) override final;
		virtual api(graphics) void draw(const image * image, const sq_rect & rect) override final;
		virtual api(graphics) void draw(const Symbol * symbol, int x, int y) override final;

		api(graphics) vertex_layout * getVertexLayout(const string & fingerprint);
		api(graphics) VertexElement * getVertexElement(const string & key);

		api(graphics) void registerVertexElement(const string & key, const string & semantic, uint units);

		template<size_t N>
		void addShaderPrograms(const RawShaderCode (&codeSets)[N])
		{
			addShaderPrograms(codeSets, N);
		}

		api(graphics) void addShaderPrograms(const RawShaderCode * codeSets, size_t count);
		api(graphics) const handle<ShaderProgram> & getShaderProgram(const string & id);

		virtual void addShaderProgram(const string & id, vertex_layout * layout, ShaderCodeSet & codeSet) = 0;

		virtual handle<vertex_buffer> createVertexBuffer(vertex_layout * layout, const vertex_data & data) = 0;
		virtual handle<mesh_builder> createMesh() = 0;

		handle<mesh> createMesh(const handle<vertex_buffer> & buffer, vertex_topology topology = vertex_topology::triangles)
		{
			return createMesh()->buffer(buffer)->topology(topology)->ready();
		}

		handle<mesh> createMesh(const handle<vertex_buffer> & buffer, const vertex_indices & indices, vertex_topology topology = vertex_topology::triangles)
		{
			return createMesh()->buffer(buffer)->indices(indices)->topology(topology)->ready();
		}

		handle<mesh> createMesh(vertex_layout * layout, const vertex_data & data, vertex_topology topology = vertex_topology::triangles)
		{
			return createMesh(createVertexBuffer(layout, data), topology);
		}

		handle<mesh> createMesh(vertex_layout * layout, const vertex_data & data, const vertex_indices & indices, vertex_topology topology = vertex_topology::triangles)
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

		state<bool> * depthTestState()
		{
			return _depthTestMode;
		}

		state<bool> * blendState()
		{
			return _blendMode;
		}

		state<bool> * accumulationState()
		{
			return _accumulationMode;
		}

		template<class U, typename ... A, useif<is_uniform<U>::value, can_construct_contents<U, A...>::value>>
		void updateUniform(A && ... args)
		{
			uniformAdapter<U>()->template update<U>(forward<A>(args)...);
		}

		template<class U, useif<is_uniform<U>::value>>
		void updateUniform(const contents<U> & contents)
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
			handle<FxTechnique> rectangle;
			handle<FxTechnique> ellipse;
			handle<FxTechnique> wired_rectangle;
			handle<FxTechnique> wired_ellipse;
			handle<FxTechnique> figure;
			handle<FxTechnique> image;
			handle<FxTechnique> text;
		} techniques2d;

		struct
		{
			handle<FxTechnique> color;
			handle<FxTechnique> multicolor;
			handle<FxTechnique> texture;
		} techniques3d;

		struct
		{
			handle<const mesh> quad;
			handle<const mesh> texquad;
			handle<const mesh> linequad;
		} meshes2d;

		struct
		{
			handle<const mesh> quad;
			handle<const mesh> texquad;
			handle<const mesh> linequad;

			handle<const mesh> cube;
			handle<const mesh> texcube;
			handle<const mesh> colorcube;
			handle<const mesh> normalcube;
			handle<const mesh> normaltexcube;
			handle<const mesh> normalcolorcube;
			handle<const mesh> linecube;
		} meshes3d;

	protected:
		friend UniformSet;
		template<class U>
		friend struct UniformChunk;

		virtual api(graphics) void initFacilities() override;
		virtual api(graphics) void updateBrushState() override;

		virtual api(graphics) void updateSurface() override;

		api(graphics) void clearFacilities();

		virtual handle<UniformAdapter> & init(handle<UniformAdapter> & adapter, const char * name, ShaderType shader, int index, size_t size) = 0;
		
		template<class U>
		UniformAdapter * uniformAdapter()
		{
			auto & a = _uniforms[morphid(U)];
			return a != nullptr ? a : init(a, U::name(), U::shader, U::index, sizeof(contents<U>));
		}

		template<class U>
		inline void uniformChunk(UniformChunk<U> & chunk);

		float _depth = 0.0f;
		StateHandle<bool> _depthTestMode {false};
		StateHandle<bool> _blendMode {false};
		StateHandle<bool> _accumulationMode {false};

		UniformSet _uniforms;

		ArrayList<Texture> _textures;

		Dictionary<string, VertexElement> _vertexElements;
		Map<string, vertex_layout> _vertexLayouts;
		Map<string, ShaderProgram> _shaderPrograms;
	};
	
	template<class U>
	struct UniformChunk : UniformData
	{
		static_assert(is_uniform<U>::value, "U must be an uniform");
		
		UniformChunk(Graphics3D * graphics) {
			graphics->uniformChunk(*this);
		}
		
		void fill(const contents<U> & contents) {
			memory<void>::move(bytes.ptr, contents.pointer(), bytes.size);
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
