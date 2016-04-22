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

	link_class(Model, Class<Object>);

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

	link_class(Graphics3D, Class<Graphics>);

	class Graphics3D : public Graphics
	{
	public:
		Graphics3D() { setclass(Graphics3D); }
		virtual ~Graphics3D() {}

		using Graphics::bind;
		virtual void bind(const Handle<Texture> & texture, uint index) = 0;

		using Graphics::draw;
		virtual void draw(const Model * model) = 0;

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

	protected:
		float _depth = 1.0f;
		StateHandle<bool> _depthTestMode {false};
	};
}

//---------------------------------------------------------------------------
#endif
