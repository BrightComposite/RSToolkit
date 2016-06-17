//---------------------------------------------------------------------------

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

		friend class Uniform;
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

		struct Meshes
		{
			Handle<Mesh, Graphics3D> quad;
			Handle<Mesh, Graphics3D> texquad;
			Handle<Mesh, Graphics3D> cube;
			Handle<Mesh, Graphics3D> texcube;
			Handle<Mesh, Graphics3D> colorcube;
		};

	public:
		Graphics3D() { setclass(Graphics3D); }
		virtual ~Graphics3D() {}

		using Graphics::bind;
		using Graphics::draw;

		void api(graphics) bind(const Handle<Texture> & texture, uint index);
		void api(graphics) bind(const VertexShader * shader);
		void api(graphics) bind(const PixelShader * shader);
		void api(graphics) bind(const VertexBuffer * buffer);
		void api(graphics) bind(const IndexBuffer * buffer);

		virtual void api(graphics) rectangle(const IntRect & rect) override final;
		virtual void api(graphics) ellipse(const IntRect & rect) override final;
		virtual void api(graphics) rectangle(const SqRect & rect) override final;
		virtual void api(graphics) ellipse(const SqRect & rect) override final;

		virtual void api(graphics) draw(const Figure * figure, const IntRect & bounds) override final;
		virtual void api(graphics) draw(const Figure * figure, const FloatTransform & transform) override final;
		virtual void api(graphics) draw(const Image * image, int x, int y) override final;
		virtual void api(graphics) draw(const Image * image, int x, int y, int width, int height) override final;
		virtual void api(graphics) draw(const Image * image, const IntRect & rect) override final;
		virtual void api(graphics) draw(const Image * image, const SqRect & rect) override final;
		virtual void api(graphics) draw(const Symbol * symbol, int x, int y) override final;

		VertexLayout api(graphics) * Graphics3D::getVertexLayout(const string & fingerprint);
		const Handle<ShaderCode> api(graphics) & getShaderCode(const string & id, ShaderType type) const;
		const Handle<ShaderProgram> api(graphics) & getShaderProgram(const string & id);

		Handle<Mesh> api(graphics) createMesh(VertexLayout * layout, const VertexData & data);
		Handle<Mesh> api(graphics) createMesh(const string & fingerprint, const VertexData & data);
		Handle<IndexedMesh> api(graphics) createIndexedMesh(VertexLayout * layout, const VertexData & data, const VertexIndices & indices, uint indicesLocation = 0);

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

		void setDepthTestMode(bool mode)
		{
			*_depthTestMode = mode;
		}

		State<bool> * depthTestModeState()
		{
			return _depthTestMode;
		}

		template<class T, typename ... A, useif <is_uniform<T>::value, can_construct_contents<T, A...>::value> endif>
		void updateUniform(A && ... args)
		{
			uniforms.require<T>(this)->template set<T>(forward<A>(args)...);
		}

		template<class T, useif <is_uniform<T>::value> endif>
		void updateUniform(const Contents<T> & contents)
		{
			uniforms.require<T>(this)->template set<T>(contents);
		}

		Techniques2D techniques2d;
		Techniques3D techniques3d;
		Meshes meshes;

	protected:
		friend UniformMap;

		virtual void api(graphics) initFacilities() override;
		virtual void api(graphics) updateBrushState() override;

		void api(graphics) updateAreaUniform(const IntRect & rect);
		void api(graphics) updateAreaUniform(const SqRect & rect);

		virtual Handle<VertexLayout> createVertexLayout(const string & fingerprint) = 0;
		virtual Handle<VertexBuffer> createVertexBuffer(VertexLayout * layout, const VertexData & data) = 0;
		virtual Handle<IndexBuffer> createIndexBuffer(const VertexIndices & indices) = 0;

		virtual UniqueHandle<UniformAdapter> createUniformAdapter(ShaderType shader, int index, size_t size) = 0;

		static Handle<ShaderCodeSet> createCodeSet(const initializer_list<Handle<ShaderCode>> & list)
		{
			Handle<ShaderCodeSet> set(nothing);

			for(size_t i = 0; i < list.size(); ++i)
				set->code.insert({static_cast<ShaderType>(i), *(list.begin() + i)});

			return set;
		}

		template<class T>
		void init(Handle<Uniform, Graphics3D> & uniform)
		{
			uniform.init(createUniformAdapter(T::shader, T::index, sizeof(Contents<T>)));
		}

		float _depth = 1.0f;
		StateHandle<bool> _depthTestMode {false};

		UniformMap uniforms;
		ShaderMap shaders;

		Array<Texture> _textures;

		HashMap<string, VertexLayout> vertexLayouts;
		HashMap<string, ShaderProgram> shaderPrograms;

		Handle<const VertexShader> _vshader;
		Handle<const PixelShader> _pshader;

		Handle<const VertexBuffer> _vbuffer;
		Handle<const IndexBuffer> _ibuffer;
	};
}

//---------------------------------------------------------------------------
#endif
