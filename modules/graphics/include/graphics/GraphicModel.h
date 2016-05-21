//---------------------------------------------------------------------------

#ifndef GRAPHIC_MODEL_H
#define GRAPHIC_MODEL_H

//---------------------------------------------------------------------------

#include <math/Transform.h>

#include "Shaders.h"
#include "Uniform.h"
#include "Mesh.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;

	class GraphicModel
	{
	public:
		GraphicModel(Graphics3D * graphics, Handle<Mesh> mesh, Handle<FxTechnique> technique) :
			graphics(graphics), mesh(mesh), technique(technique) {}

		const FloatTransform & getTransform() const
		{
			return transform;
		}

		void setTransform(const FloatTransform & t)
		{
			transform = t;
		}

		void setTransform(FloatTransform && t)
		{
			transform = forward<FloatTransform>(t);
		}

		virtual void draw(int pass = 0) const;

	protected:
		Graphics3D * graphics;
		Handle<Mesh> mesh;
		Handle<FxTechnique> technique;
		FloatTransform transform;
	};

	class Figure3D : public Figure
	{
	public:
		Figure3D(Graphics3D * graphics, const FigureData & data);

		virtual void draw() const override
		{
			model.draw();
		}

		GraphicModel model;
	};
}

//---------------------------------------------------------------------------
#endif
