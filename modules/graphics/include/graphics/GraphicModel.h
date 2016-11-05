//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHIC_MODEL_H
#define GRAPHIC_MODEL_H

//---------------------------------------------------------------------------

#include <math/Transform.h>

#include "Shaders.h"
#include "Uniform.h"
#include "Mesh.h"

//---------------------------------------------------------------------------

namespace asd
{
	class Graphics3D;

	class GraphicModelView
	{
	public:
		GraphicModelView(const Handle<const Mesh> & mesh, const Handle<FxTechnique> & technique) :
			mesh(mesh), technique(technique) {}

		Handle<const Mesh> mesh;
		Handle<FxTechnique> technique;
	};

	class GraphicModel : public Shared
	{
	public:
		GraphicModel(Graphics3D * graphics, const Handle<const Mesh> & mesh, const Handle<FxTechnique> & technique) :
			graphics(graphics), view(mesh, technique) {}

		Graphics3D * graphics;
		GraphicModelView view;
	};

	class Figure3D : public Figure
	{
	public:
		api(graphics) Figure3D(Graphics3D * graphics, const FigureData & data);

		virtual void draw() const override
		{
			view.technique->apply();
			view.mesh->draw();
		}

		GraphicModelView view;
	};
}

//---------------------------------------------------------------------------
#endif
