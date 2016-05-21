//---------------------------------------------------------------------------

#ifndef GRAPHICS_3D_H
#define GRAPHICS_3D_H

//---------------------------------------------------------------------------

#include "Graphics.h" 
#include "GraphicModel.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	declare_and_link(Graphics3D, Class<Graphics>);

	using Texture = Image;

	class Graphics3D : public Graphics
	{
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
			Handle<FxTechnique, Graphics3D> texture;
		};

		struct Meshes
		{
			Handle<Mesh, Graphics3D> quad;
			Handle<Mesh, Graphics3D> texquad;
			Handle<Mesh, Graphics3D> cube;
			Handle<Mesh, Graphics3D> texcube;
		};

	public:
		Graphics3D() { setclass(Graphics3D); }
		virtual ~Graphics3D() {}

		using Graphics::bind;
		using Graphics::draw;

		void bind(const Handle<Texture> & texture, uint index)
		{
			if(_textures.size() > index && _textures[index] == texture)
				return;

			_textures[index] = texture;
			texture->apply();
		}

		void bind(const VertexShader * shader)
		{
			if(_vshader != shader)
			{
				_vshader = shader;
				_vshader->apply();
			}
		}

		void bind(const PixelShader * shader)
		{
			if(_pshader != shader)
			{
				_pshader = shader;
				_pshader->apply();
			}
		}

		void bind(const VertexBuffer * buffer)
		{
			if(_vbuffer != buffer)
			{
				_vbuffer = buffer;
				_vbuffer->apply();
			}
		}

		void bind(const IndexBuffer * buffer)
		{
			if(_ibuffer != buffer)
			{
				_ibuffer = buffer;
				_ibuffer->apply();
			}
		}

		virtual void rectangle(const IntRect & rect) override final
		{
			auto fm = hold(_fillMode, FillMode::Solid);

			updateAreaUniform(rect);

			if(fm.old == FillMode::Solid)
				techniques2d.rectangle->apply();
			else
				techniques2d.wired_rectangle->apply();

			draw(meshes.quad);
		}

		virtual void ellipse(const IntRect & rect) override final
		{
			auto fm = hold(_fillMode, FillMode::Solid);

			updateAreaUniform(rect);

			if(fm.old == FillMode::Solid)
				techniques2d.ellipse->apply();
			else
				techniques2d.wired_ellipse->apply();

			draw(meshes.texquad);
		}

		virtual void rectangle(const SqRect & rect) override final
		{
			auto fm = hold(_fillMode, FillMode::Solid);

			updateAreaUniform(rect);

			if(fm.old == FillMode::Solid)
				techniques2d.rectangle->apply();
			else
				techniques2d.wired_rectangle->apply();

			draw(meshes.texquad);
		}

		virtual void ellipse(const SqRect & rect) override final
		{
			auto fm = hold(_fillMode, FillMode::Solid);

			auto s = rect.size();
			updateUniform<Uniforms::Area>(rect.pos() + s * 0.5f, s, _depth);

			if(fm.old == FillMode::Solid)
				techniques2d.ellipse->apply();
			else
				techniques2d.wired_ellipse->apply();

			draw(meshes.texquad);
		}

		virtual void draw(const Figure * figure, const IntRect & bounds) override final
		{
			FloatPoint t;
			FloatSize s;

			ScreenCoord::toRel(bounds, viewport(), t, s);

			updateUniform<Uniforms::Model>(FloatTransform(move(t), move(s), _depth));
			techniques2d.figure->apply();

			figure->draw();
		}

		virtual void draw(const Figure * figure, const FloatTransform & transform) override final
		{
			updateUniform<Uniforms::Model>(transform);
			techniques2d.figure->apply();

			figure->draw();
		}

		virtual void draw(const Image * image, int x, int y) override final
		{
			draw(image, IntRect{x, y, x + image->width(), y + image->height()});
		}

		virtual void draw(const Image * image, int x, int y, int width, int height) override final
		{
			draw(image, IntRect{x, y, x + width, y + height});
		}

		virtual void draw(const Image * image, const IntRect & rect) override final
		{
			if(image->graphics() != this)
				return;

			image->apply();
			updateAreaUniform(rect);

			techniques2d.image->apply();
			draw(meshes.texquad);
		}

		virtual void draw(const Image * image, const SqRect & rect) override final
		{
			if(image->graphics() != this)
				return;

			image->apply();
			updateAreaUniform(rect);

			techniques2d.image->apply();
			draw(meshes.texquad);
		}

		virtual void draw(const Symbol * symbol, int x, int y) override final
		{
			if(symbol->graphics() != this)
				return;

			auto image = symbol->image();

			if(image == nullptr)
				return;

			image->apply();

			updateAreaUniform(IntRect({x + symbol->left(), y + symbol->top()}, {image->width(), image->height()}));

			techniques2d.text->apply();
			draw(meshes.texquad);
		}

		void draw(const Mesh * mesh)
		{
			mesh->draw(this);
		}

		const Handle<ShaderCode> & getShaderCode(const string & id, ShaderType type) const
		{
			auto i = shaders.find(id);

			if(i == shaders.end())
				throw Exception("Can't find embedded shader set with id \"", id, "\"");

			auto & set = i->second;
			auto ci = set->code.find(type);

			if(ci == set->code.end())
				throw Exception("Embedded shader set with id \"", id, "\" doesn't contain shader of type \"", type, "\"");

			return ci->second;
		}

		VertexLayout * Graphics3D::getVertexLayout(const string & fingerprint)
		{
			auto & layout = vertexLayouts[fingerprint];

			if(layout == nullptr)
				layout = createVertexLayout(fingerprint);

			return layout;
		}

		const Handle<ShaderProgram> & getShaderProgram(const string & id)
		{
			return shaderPrograms[id];
		}

		Handle<Mesh> createMesh(VertexLayout * vil, const VertexData & data)
		{
			return Handle<Mesh>(createVertexBuffer(vil, data), vil->stride, data.start);
		}

		Handle<Mesh> createMesh(const string & fingerprint, const VertexData & data)
		{
			auto vil = getVertexLayout(fingerprint);
			return Handle<Mesh>(createVertexBuffer(vil, data), vil->stride, data.start);
		}

		Handle<IndexedMesh> createIndexedMesh(VertexLayout * vil, const VertexData & data, const VertexIndices & indices, uint indicesLocation = 0)
		{
			return Handle<IndexedMesh>(createVertexBuffer(vil, data), createIndexBuffer(indices), vil->stride, data.start, indicesLocation);
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

		void updateAreaUniform(const IntRect & rect)
		{
			FloatPoint t;
			FloatSize s;

			ScreenCoord::toRel(rect, viewport(), t, s);
			updateUniform<Uniforms::Area>(move(t), move(s), _depth);
		}

		void updateAreaUniform(const SqRect & rect)
		{
			FloatPoint t;
			FloatSize s;

			ScreenCoord::toRel(rect, viewport(), t, s);
			updateUniform<Uniforms::Area>(move(t), move(s), _depth);
		}

		virtual Handle<VertexLayout> createVertexLayout(const string & fingerprint) = 0;
		virtual Handle<VertexBuffer> createVertexBuffer(VertexLayout * vil, const VertexData & data) = 0;
		virtual Handle<IndexBuffer> createIndexBuffer(const VertexIndices & indices) = 0;

		virtual Handle<UniformAdapter> createUniformAdapter(ShaderType shader, int index, size_t size) = 0;

		virtual void initFacilities() override
		{
			auto p2  = getVertexLayout("p2");
			auto p2t = getVertexLayout("p2 t");
			auto p3  = getVertexLayout("p3");
			auto p3t = getVertexLayout("p3 t");

			meshes.quad		= createMesh(p2, VertexData::quad);
			meshes.texquad	= createMesh(p2t, VertexData::texquad);
			meshes.cube		= createIndexedMesh(p3, VertexData::cube, VertexIndices::cube);
			meshes.texcube	= createIndexedMesh(p3t, VertexData::texcube, VertexIndices::cube);

			techniques2d.rectangle.			init(getShaderProgram("2d/rect"));
			techniques2d.ellipse.			init(getShaderProgram("2d/ellipse"));
			techniques2d.wired_rectangle.	init(getShaderProgram("2d/wired/rect"));
			techniques2d.wired_ellipse.		init(getShaderProgram("2d/wired/ellipse"));
			techniques2d.figure.			init(getShaderProgram("2d/figure"));
			techniques2d.image.				init(getShaderProgram("2d/image"));
			techniques2d.text.				init(getShaderProgram("2d/text"));

			techniques3d.color.				init(getShaderProgram("3d/color"));
			techniques3d.texture.			init(getShaderProgram("3d/texture"));

			updateUniform<Uniforms::Model>(fmat());
			updateUniform<Uniforms::View>(fmat());
			updateUniform<Uniforms::Projection>(fmat());

			techniques2d.rectangle->apply();
		}

		static Handle<ShaderCodeSet> createCodeSet(const initializer_list<Handle<ShaderCode>> & list)
		{
			Handle<ShaderCodeSet> set(emptiness);

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
