//---------------------------------------------------------------------------

#include <graphics/Graphics3D.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	static const float quadData[] =
	{
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,
		 1.0f, -1.0f,
	};

	static const float texquadData[] =
	{
		-1.0f,  1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,	1.0f, 1.0f,
		-1.0f, -1.0f,	0.0f, 1.0f,
		-1.0f,  1.0f,	0.0f, 0.0f,
		 1.0f,  1.0f,	1.0f, 0.0f,
		 1.0f, -1.0f,	1.0f, 1.0f,
	};

	static const float cubeData[] =
	{
		/*-FRONT--*/	-1.0f,  1.0f, -1.0f, /**/  1.0f, -1.0f, -1.0f, /**/ -1.0f, -1.0f, -1.0f, /**/  1.0f,  1.0f, -1.0f,
		/*--BACK--*/	 1.0f,  1.0f,  1.0f, /**/ -1.0f, -1.0f,  1.0f, /**/  1.0f, -1.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f,
		/*--LEFT--*/	-1.0f,  1.0f,  1.0f, /**/ -1.0f, -1.0f,  1.0f, /**/ -1.0f, -1.0f, -1.0f, /**/ -1.0f,  1.0f, -1.0f,
		/*-RIGHT--*/	 1.0f,  1.0f, -1.0f, /**/  1.0f, -1.0f, -1.0f, /**/  1.0f, -1.0f,  1.0f, /**/  1.0f,  1.0f,  1.0f,
		/*--TOP---*/	-1.0f,  1.0f,  1.0f, /**/ -1.0f,  1.0f, -1.0f, /**/  1.0f,  1.0f, -1.0f, /**/  1.0f,  1.0f,  1.0f,
		/*--TOP---*/	-1.0f, -1.0f, -1.0f, /**/ -1.0f, -1.0f,  1.0f, /**/  1.0f, -1.0f,  1.0f, /**/  1.0f, -1.0f, -1.0f,
	};

	static const float texcubeData[] =
	{
		/*-FRONT--*/	-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, /**/  1.0f, -1.0f, -1.0f, 0.0f, 1.0f, /**/ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, /**/  1.0f,  1.0f, -1.0f, 1.0f, 0.0f,
		/*--BACK--*/	 1.0f,  1.0f,  1.0f, 0.0f, 0.0f, /**/ -1.0f, -1.0f,  1.0f, 0.0f, 1.0f, /**/  1.0f, -1.0f,  1.0f, 1.0f, 1.0f, /**/ -1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
		/*--LEFT--*/	-1.0f,  1.0f,  1.0f, 0.0f, 0.0f, /**/ -1.0f, -1.0f,  1.0f, 0.0f, 1.0f, /**/ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, /**/ -1.0f,  1.0f, -1.0f, 1.0f, 0.0f,
		/*-RIGHT--*/	 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, /**/  1.0f, -1.0f, -1.0f, 0.0f, 1.0f, /**/  1.0f, -1.0f,  1.0f, 1.0f, 1.0f, /**/  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
		/*--TOP---*/	-1.0f,  1.0f,  1.0f, 0.0f, 0.0f, /**/ -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, /**/  1.0f,  1.0f, -1.0f, 1.0f, 1.0f, /**/  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
		/*-BOTTOM-*/	-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, /**/ -1.0f, -1.0f,  1.0f, 0.0f, 1.0f, /**/  1.0f, -1.0f,  1.0f, 1.0f, 1.0f, /**/  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
	};

#define indices(o, x0, x1, x2, x3, x4, x5) x0 + o, x1 + o, x2 + o, x3 + o, x4 + o, x5 + o

	VertexIndices VertexIndices::cube = {
		indices(0,  0, 1, 2, 0, 3, 2),
		indices(4,  0, 1, 2, 0, 3, 2),
		indices(8,  0, 1, 2, 0, 3, 2),
		indices(12, 0, 1, 2, 0, 3, 2),
		indices(16, 0, 1, 2, 0, 3, 2),
		indices(20, 0, 1, 2, 0, 3, 2)
	};

#undef indices

	VertexData VertexData::quad    (quadData);
	VertexData VertexData::texquad (texquadData);
	VertexData VertexData::cube    (cubeData);
	VertexData VertexData::texcube (texcubeData);

	VertexElement VertexElement::pos2            ("p2", "POSITION", 0, 2);
	VertexElement VertexElement::pos3            ("p3", "POSITION", 0, 3);
	VertexElement VertexElement::color3          ("c3", "COLOR",    0, 3);
	VertexElement VertexElement::colorf          ("c4", "COLOR",    0, 4);
	VertexElement VertexElement::tex             ("t",  "TEXCOORD", 0, 2);
	VertexElement VertexElement::normal          ("n",  "NORMAL",   0, 3);
	VertexElement VertexElement::secondaryColor3 ("s3", "COLOR",    1, 3);
	VertexElement VertexElement::secondaryColor4 ("s4", "COLOR",    1, 4);

	void GraphicModel::draw(int pass) const
	{
		graphics->updateUniform<Uniforms::Model>(getTransform());

		technique->apply(pass);
		graphics->draw(mesh);
	}

	void Mesh::draw(Graphics3D * graphics) const
	{
		graphics->bind(vbuffer);
		vbuffer->draw(this);
	}

	void IndexedMesh::draw(Graphics3D * graphics) const
	{
		graphics->bind(vbuffer);
		graphics->bind(ibuffer);
		ibuffer->draw(this);
	}

	static Handle<Mesh> createMesh(Graphics3D * graphics, const FigureData & data)
	{
		vector<FloatPoint> points;
		auto & p = data.points;

		for(int i = 0; i < p.size() - 3; i += 2)
		{
			points.push_back(p[i]);
			points.push_back(p[i + 1]);
			points.push_back(p[i + 2]);
		}

		points.push_back(p[p.size() - 2]);
		points.push_back(p[p.size() - 1]);
		points.push_back(p[0]);

		return graphics->createMesh("p2", points);
	}

	Figure3D::Figure3D(Graphics3D * graphics, const FigureData & data) : Figure(graphics, data), model(graphics, createMesh(graphics, data), graphics->techniques2d.figure) {}
}

//---------------------------------------------------------------------------
