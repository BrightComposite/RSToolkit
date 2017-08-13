//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHIC_MODEL_H
#define GRAPHIC_MODEL_H

//---------------------------------------------------------------------------

#include <math/transform.h>

#include "Shaders.h"
#include "Uniform.h"
#include "mesh.h"

//---------------------------------------------------------------------------

namespace asd
{
	class Graphics3D;

	class GraphicModelView
	{
	public:
		GraphicModelView(const handle<const mesh> & mesh, const handle<FxTechnique> & technique) :
			mesh(mesh), technique(technique) {}

		handle<const asd::mesh> mesh;
		handle<FxTechnique> technique;
	};

	class GraphicModel : public shareable
	{
	public:
		GraphicModel(Graphics3D * graphics, const handle<const mesh> & mesh, const handle<FxTechnique> & technique) :
			graphics(graphics), view(mesh, technique) {}

		Graphics3D * graphics;
		GraphicModelView view;
	};

	class Figure3D : public Figure
	{
	public:
		api(graphics) Figure3D(Graphics3D * graphics, const figure_data & data);

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
