//---------------------------------------------------------------------------

#ifndef GRAPHICS_3D_H
#define GRAPHICS_3D_H

//---------------------------------------------------------------------------

#include "Graphics.h"
#include "VertexLayout.h"
#include "Shaders.h"
#include "Uniforms.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;
	class VertexData;
	class Model;
	class Uniform;

	link_class(Graphics3D, Class<Graphics>);
	link_class(Model, Class<Object>);

	using Texture = Image;

	class VertexData : public RawData<const void>
	{
	public:
		using RawData<const void>::RawData;

		static VertexData quad;
		static VertexData texquad;

		uint start = 0;
	};

	using VertexIndices = vector<uint16_t>;

	class Model : public Object
	{
	public:
		Model(const Graphics3D * graphics, const VertexData & vertexData, uint stride) : graphics(graphics), stride(stride), verticesLocation(vertexData.start)
		{
			setclass(Model);
		}

		const Graphics3D * graphics;
		uint stride, verticesLocation;
	};

	class Graphics3D : public Graphics
	{
		friend class Uniform;

	public:
		Graphics3D() { setclass(Graphics3D); }
		virtual ~Graphics3D() {}

		using Graphics::bind;
		using Graphics::draw;

		virtual void bind(const Handle<Texture> & texture, uint index) = 0;
		virtual void draw(const Model * model) = 0;

		virtual Handle<ShaderCode> getShaderCode(const string & id, ShaderType type) = 0;

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

	protected:
		friend UniformMap;
		virtual Handle<UniformAdapter> createUniformAdapter(ShaderType shader, int index, size_t size) = 0;

		template<class T>
		void init(Handle<Uniform, Graphics3D> & uniform)
		{
			uniform.init(createUniformAdapter(T::shader, T::index, sizeof(Contents<T>)));
		}

		float _depth = 1.0f;
		StateHandle<bool> _depthTestMode {false};
		UniformMap uniforms;
	};
}

//---------------------------------------------------------------------------
#endif
