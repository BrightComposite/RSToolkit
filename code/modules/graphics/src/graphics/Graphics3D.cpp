//---------------------------------------------------------------------------

#include <graphics/Graphics3D.h>

//---------------------------------------------------------------------------

namespace asd
{
	implement_link(Graphics3D);

	const VertexData VertexData::quad2d({
		-1.0f,  1.0f,
		 1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f, -1.0f
	});

	const VertexData VertexData::texquad2d({
		-1.0f,  1.0f,	0.0f, 0.0f,
		 1.0f,  1.0f,	1.0f, 0.0f,
		 1.0f, -1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,	1.0f, 1.0f,
		-1.0f, -1.0f,	0.0f, 1.0f
	});

	const VertexData VertexData::linequad2d({
		-1.0f,  1.0f,
		 1.0f,  1.0f,
		 1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f,  1.0f
	});

	const VertexData VertexData::quad3d({
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f
	});

	const VertexData VertexData::texquad3d({
		-1.0f,  1.0f, 0.0f,		0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,		1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,		1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,		0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,		1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 1.0f
	});

	const VertexData VertexData::linequad3d({
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f
	});

	const VertexData VertexData::cube({
		/* Front  */ -1.0f, -1.0f,  1.0f, /**/  1.0f, -1.0f,  1.0f, /**/  1.0f,  1.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f,
		/*  Back  */ -1.0f, -1.0f, -1.0f, /**/ -1.0f,  1.0f, -1.0f, /**/  1.0f,  1.0f, -1.0f, /**/  1.0f, -1.0f, -1.0f,

		/*  Top   */ -1.0f,  1.0f, -1.0f, /**/ -1.0f,  1.0f,  1.0f, /**/  1.0f,  1.0f,  1.0f, /**/  1.0f,  1.0f, -1.0f,
		/* Bottom */ -1.0f, -1.0f, -1.0f, /**/  1.0f, -1.0f, -1.0f, /**/  1.0f, -1.0f,  1.0f, /**/ -1.0f, -1.0f,  1.0f,

		/* Right  */  1.0f, -1.0f, -1.0f, /**/  1.0f,  1.0f, -1.0f, /**/  1.0f,  1.0f,  1.0f, /**/  1.0f, -1.0f,  1.0f,
		/*  Left  */ -1.0f, -1.0f, -1.0f, /**/ -1.0f, -1.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f, /**/ -1.0f,  1.0f, -1.0f
	});

	const VertexData VertexData::texcube({
		/* Front  */ -1.0f, -1.0f,  1.0f,   1.0f,  1.0f, /**/  1.0f, -1.0f,  1.0f,   0.0f,  1.0f, /**/  1.0f,  1.0f,  1.0f,   0.0f,  0.0f, /**/ -1.0f,  1.0f,  1.0f,   1.0f,  0.0f,
		/*  Back  */ -1.0f, -1.0f, -1.0f,   0.0f,  1.0f, /**/ -1.0f,  1.0f, -1.0f,   0.0f,  0.0f, /**/  1.0f,  1.0f, -1.0f,   1.0f,  0.0f, /**/  1.0f, -1.0f, -1.0f,   1.0f,  1.0f,

		/*  Top   */ -1.0f,  1.0f, -1.0f,   0.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f,   0.0f,  0.0f, /**/  1.0f,  1.0f,  1.0f,   1.0f,  1.0f, /**/  1.0f,  1.0f, -1.0f,   1.0f,  0.0f,
		/* Bottom */ -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, /**/  1.0f, -1.0f, -1.0f,   1.0f,  0.0f, /**/  1.0f, -1.0f,  1.0f,   1.0f,  1.0f, /**/ -1.0f, -1.0f,  1.0f,   0.0f,  1.0f,

		/* Right  */  1.0f, -1.0f, -1.0f,   0.0f,  1.0f, /**/  1.0f,  1.0f, -1.0f,   0.0f,  0.0f, /**/  1.0f,  1.0f,  1.0f,   1.0f,  0.0f, /**/  1.0f, -1.0f,  1.0f,   1.0f,  1.0f,
		/*  Left  */ -1.0f, -1.0f, -1.0f,   1.0f,  1.0f, /**/ -1.0f, -1.0f,  1.0f,   0.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f,   0.0f,  0.0f, /**/ -1.0f,  1.0f, -1.0f,   1.0f,  0.0f
	});

	const VertexData VertexData::colorcube({
		/* Front  */ -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f, /**/  1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f, /**/  1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f,
		/*  Back  */ -1.0f, -1.0f, -1.0f,   1.0f,  1.0f,  0.0f,  1.0f, /**/ -1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  0.0f,  1.0f, /**/  1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  0.0f,  1.0f, /**/  1.0f, -1.0f, -1.0f,   1.0f,  1.0f,  0.0f,  1.0f,

		/*  Top   */ -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,  1.0f, /**/  1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,  1.0f, /**/  1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,  1.0f,
		/* Bottom */ -1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  1.0f,  1.0f, /**/  1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  1.0f,  1.0f, /**/  1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  1.0f,  1.0f, /**/ -1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  1.0f,  1.0f,

		/* Right  */  1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,  1.0f, /**/  1.0f,  1.0f, -1.0f,   1.0f,  0.0f,  0.0f,  1.0f, /**/  1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,  1.0f, /**/  1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  0.0f,  1.0f,
		/*  Left  */ -1.0f, -1.0f, -1.0f,   0.0f,  1.0f,  1.0f,  1.0f, /**/ -1.0f, -1.0f,  1.0f,   0.0f,  1.0f,  1.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  1.0f,  1.0f, /**/ -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  1.0f,  1.0f
	});

	const VertexData VertexData::normalcube({
		/* Front  */ -1.0f, -1.0f,  1.0f,	0.0f,  0.0f,  1.0f,	/**/  1.0f, -1.0f,  1.0f, 	0.0f,  0.0f,  1.0f,	/**/  1.0f,  1.0f,  1.0f, 	0.0f,  0.0f,  1.0f,	/**/ -1.0f,  1.0f,  1.0f,	0.0f,  0.0f,  1.0f, 
		/*  Back  */ -1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,	/**/ -1.0f,  1.0f, -1.0f,	0.0f,  0.0f, -1.0f,	/**/  1.0f,  1.0f, -1.0f,	0.0f,  0.0f, -1.0f,	/**/  1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,	

		/*  Top   */ -1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,	/**/ -1.0f,  1.0f,  1.0f,	0.0f,  1.0f,  0.0f,	/**/  1.0f,  1.0f,  1.0f,	0.0f,  1.0f,  0.0f,	/**/  1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,	
		/* Bottom */ -1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,	/**/  1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,	/**/  1.0f, -1.0f,  1.0f,	0.0f, -1.0f,  0.0f,	/**/ -1.0f, -1.0f,  1.0f,	0.0f, -1.0f,  0.0f,	

		/* Right  */  1.0f, -1.0f, -1.0f,	1.0f,  0.0f,  0.0f,	/**/  1.0f,  1.0f, -1.0f, 	1.0f,  0.0f,  0.0f,	/**/  1.0f,  1.0f,  1.0f, 	1.0f,  0.0f,  0.0f,	/**/  1.0f, -1.0f,  1.0f,	1.0f,  0.0f,  0.0f,	
		/*  Left  */ -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,	/**/ -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,	/**/ -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,	/**/ -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f
	});

	const VertexData VertexData::normaltexcube({
		/* Front  */ -1.0f, -1.0f,  1.0f,	0.0f,  0.0f,  1.0f,	   1.0f,  1.0f, /**/  1.0f, -1.0f,  1.0f,	0.0f,  0.0f,  1.0f,	   0.0f,  1.0f, /**/  1.0f,  1.0f,  1.0f,	0.0f,  0.0f,  1.0f,	   0.0f,  0.0f, /**/ -1.0f,  1.0f,  1.0f,	0.0f,  0.0f,  1.0f,	   1.0f,  0.0f,
		/*  Back  */ -1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,    0.0f,  1.0f, /**/ -1.0f,  1.0f, -1.0f,	0.0f,  0.0f, -1.0f,    0.0f,  0.0f, /**/  1.0f,  1.0f, -1.0f,	0.0f,  0.0f, -1.0f,    1.0f,  0.0f, /**/  1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,    1.0f,  1.0f,

		/*  Top   */ -1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,	   0.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f,	0.0f,  1.0f,  0.0f,	   0.0f,  0.0f, /**/  1.0f,  1.0f,  1.0f,	0.0f,  1.0f,  0.0f,	   1.0f,  1.0f, /**/  1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,	   1.0f,  0.0f,
		/* Bottom */ -1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,	   0.0f,  0.0f, /**/  1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,	   1.0f,  0.0f, /**/  1.0f, -1.0f,  1.0f,	0.0f, -1.0f,  0.0f,	   1.0f,  1.0f, /**/ -1.0f, -1.0f,  1.0f,	0.0f, -1.0f,  0.0f,	   0.0f,  1.0f,

		/* Right  */  1.0f, -1.0f, -1.0f,	1.0f,  0.0f,  0.0f,	   0.0f,  1.0f, /**/  1.0f,  1.0f, -1.0f,	1.0f,  0.0f,  0.0f,	   0.0f,  0.0f, /**/  1.0f,  1.0f,  1.0f,	1.0f,  0.0f,  0.0f,	   1.0f,  0.0f, /**/  1.0f, -1.0f,  1.0f,	1.0f,  0.0f,  0.0f,	   1.0f,  1.0f,
		/*  Left  */ -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,	   1.0f,  1.0f, /**/ -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,	   0.0f,  1.0f, /**/ -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,	   0.0f,  0.0f, /**/ -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,	   1.0f,  0.0f
	});

	const VertexData VertexData::normalcolorcube({
		/* Front  */ -1.0f, -1.0f,  1.0f,	0.0f,  0.0f,  1.0f,	   0.0f,  0.0f,  1.0f,  1.0f,	/**/  1.0f, -1.0f,  1.0f,	0.0f,  0.0f,  1.0f,	   0.0f,  0.0f,  1.0f,  1.0f,	/**/  1.0f,  1.0f,  1.0f,	0.0f,  0.0f,  1.0f,	   0.0f,  0.0f,  1.0f,  1.0f,	/**/ -1.0f,  1.0f,  1.0f,	0.0f,  0.0f,  1.0f,	   0.0f,  0.0f,  1.0f,  1.0f,
		/*  Back  */ -1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,    1.0f,  1.0f,  0.0f,  1.0f,	/**/ -1.0f,  1.0f, -1.0f,	0.0f,  0.0f, -1.0f,    1.0f,  1.0f,  0.0f,  1.0f,	/**/  1.0f,  1.0f, -1.0f,	0.0f,  0.0f, -1.0f,    1.0f,  1.0f,  0.0f,  1.0f,	/**/  1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,    1.0f,  1.0f,  0.0f,  1.0f,

		/*  Top   */ -1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,	   0.0f,  1.0f,  0.0f,  1.0f,	/**/ -1.0f,  1.0f,  1.0f,	0.0f,  1.0f,  0.0f,	   0.0f,  1.0f,  0.0f,  1.0f,	/**/  1.0f,  1.0f,  1.0f,	0.0f,  1.0f,  0.0f,	   0.0f,  1.0f,  0.0f,  1.0f,	/**/  1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,	   0.0f,  1.0f,  0.0f,  1.0f,
		/* Bottom */ -1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,	   1.0f,  0.0f,  1.0f,  1.0f,	/**/  1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,	   1.0f,  0.0f,  1.0f,  1.0f,	/**/  1.0f, -1.0f,  1.0f,	0.0f, -1.0f,  0.0f,	   1.0f,  0.0f,  1.0f,  1.0f,	/**/ -1.0f, -1.0f,  1.0f,	0.0f, -1.0f,  0.0f,	   1.0f,  0.0f,  1.0f,  1.0f,

		/* Right  */  1.0f, -1.0f, -1.0f,	1.0f,  0.0f,  0.0f,	   1.0f,  0.0f,  0.0f,  1.0f,	/**/  1.0f,  1.0f, -1.0f,	1.0f,  0.0f,  0.0f,	   1.0f,  0.0f,  0.0f,  1.0f,	/**/  1.0f,  1.0f,  1.0f,	1.0f,  0.0f,  0.0f,	   1.0f,  0.0f,  0.0f,  1.0f,	/**/  1.0f, -1.0f,  1.0f,	1.0f,  0.0f,  0.0f,	   1.0f,  0.0f,  0.0f,  1.0f,
		/*  Left  */ -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,	   0.0f,  1.0f,  1.0f,  1.0f,	/**/ -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,	   0.0f,  1.0f,  1.0f,  1.0f,	/**/ -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,	   0.0f,  1.0f,  1.0f,  1.0f,	/**/ -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,	   0.0f,  1.0f,  1.0f,  1.0f
	});

#define indices(o, x0, x1, x2, x3, x4, x5) x0 + o, x1 + o, x2 + o, x3 + o, x4 + o, x5 + o

	VertexIndices VertexIndices::cube = {
		indices(0,  0, 1, 2, 0, 2, 3),
		indices(4,  0, 1, 2, 0, 2, 3),
		indices(8,  0, 1, 2, 0, 2, 3),
		indices(12, 0, 1, 2, 0, 2, 3),
		indices(16, 0, 1, 2, 0, 2, 3),
		indices(20, 0, 1, 2, 0, 2, 3)
	};

#undef indices

#define indices(o, x0, x1, x2, x3, x4, x5, x6, x7) x0 + o, x1 + o, x2 + o, x3 + o, x4 + o, x5 + o, x6 + o, x7 + o

	VertexIndices VertexIndices::linecube = {
		indices(0,  0, 1, 1, 2, 2, 3, 3, 0),
		indices(4,  0, 1, 1, 2, 2, 3, 3, 0),
		indices(8,  0, 1, 1, 2, 2, 3, 3, 0),
		indices(12, 0, 1, 1, 2, 2, 3, 3, 0),
		indices(16, 0, 1, 1, 2, 2, 3, 3, 0),
		indices(20, 0, 1, 1, 2, 2, 3, 3, 0)
	};

#undef indices

	void UniformData::apply() const
	{
		adapter->bind(*this);
	}

	VertexLayout::VertexLayout(Graphics3D * graphics, const string & fingerprint) : fingerprint(fingerprint), units(0)
	{
		for(const auto & key : split(fingerprint))
		{
			auto * e = graphics->getVertexElement(key);
			elements.push_back(e);
			units += e->units;
		}

		stride = units * sizeof(float);
	}

	static Handle<const Mesh> createMesh(Graphics3D * graphics, const FigureData & data)
	{
		VertexData points;
		auto & p = data.points;

		for(size_t i = 0; i < p.size() - 3; i += 2)
		{
			points.push_back(p[i].x);
			points.push_back(p[i].y);
			points.push_back(p[i + 1].x);
			points.push_back(p[i + 1].y);
			points.push_back(p[i + 2].x);
			points.push_back(p[i + 2].y);
		}

		points.push_back(p[p.size() - 2].x);
		points.push_back(p[p.size() - 2].y);
		points.push_back(p[p.size() - 1].x);
		points.push_back(p[p.size() - 1].y);
		points.push_back(p[0].x);
		points.push_back(p[0].y);

		return graphics->createMesh(graphics->getVertexLayout("p2"), points);
	}

	Figure3D::Figure3D(Graphics3D * graphics, const FigureData & data) : Figure(graphics, data), view(createMesh(graphics, data), graphics->techniques2d.figure) {}

	Graphics3D::Graphics3D()
	{ 
		setclass(Graphics3D); 

		registerVertexElement("p2",   "position", 2);
		registerVertexElement("p3",   "position", 3);
		registerVertexElement("c3",   "color",	  3);
		registerVertexElement("c4",   "color",    4);
		registerVertexElement("t",    "texcoord", 2);
		registerVertexElement("n",    "normal",   3);
		registerVertexElement("c3-0", "color0",   3);
		registerVertexElement("c4-0", "color0",   4);
		registerVertexElement("c3-1", "color1",   3);
		registerVertexElement("c4-1", "color1",   4);
		registerVertexElement("c3-2", "color2",   3);
		registerVertexElement("c4-2", "color2",   4);
		registerVertexElement("c3-3", "color3",   3);
		registerVertexElement("c4-3", "color3",   4);
	}

	void Graphics3D::bind(const Handle<Texture> & texture, uint index)
	{
		if(_textures.size() > index && _textures[index] == texture)
			return;

		_textures[index] = texture;
		texture->apply();
	}

	void Graphics3D::rectangle(const IntRect & rect)
	{
		auto fm = hold(_fillMode, FillMode::Solid);

		setArea(rect);

		if(fm.old == FillMode::Solid)
		{
			techniques2d.rectangle->apply();
			meshes2d.quad->draw();
		}
		else
		{
			techniques2d.wired_rectangle->apply();
			meshes2d.texquad->draw();
		}
	}

	void Graphics3D::ellipse(const IntRect & rect)
	{
		auto fm = hold(_fillMode, FillMode::Solid);

		setArea(rect);

		if(fm.old == FillMode::Solid)
			techniques2d.ellipse->apply();
		else
			techniques2d.wired_ellipse->apply();

		meshes2d.texquad->draw();
	}

	void Graphics3D::rectangle(const SqRect & rect)
	{
		auto fm = hold(_fillMode, FillMode::Solid);

		setArea(rect);

		if(fm.old == FillMode::Solid)
			techniques2d.rectangle->apply();
		else
			techniques2d.wired_rectangle->apply();

		meshes2d.quad->draw();
	}

	void Graphics3D::ellipse(const SqRect & rect)
	{
		auto fm = hold(_fillMode, FillMode::Solid);

		auto s = rect.size();
		updateUniform<Uniforms::Area>(rect.pos() + s * 0.5f, s, _depth);

		if(fm.old == FillMode::Solid)
			techniques2d.ellipse->apply();
		else
			techniques2d.wired_ellipse->apply();

		meshes2d.texquad->draw();
	}

	void Graphics3D::draw(const Figure * figure, const IntRect & bounds)
	{
		FloatPoint t;
		FloatSize s;

		ScreenCoord::toRel(bounds, viewport(), t, s);

		updateUniform<Uniforms::Model>(FloatTransform(move(t), move(s), _depth).output());
		techniques2d.figure->apply();

		figure->draw();
	}

	void Graphics3D::draw(const Figure * figure, const FloatTransform & transform)
	{
		updateUniform<Uniforms::Model>(transform.output());
		techniques2d.figure->apply();

		figure->draw();
	}

	void Graphics3D::draw(const Image * image, const IntRect & rect)
	{
		if(image->graphics() != this)
			return;

		image->apply();
		setArea(rect);

		techniques2d.image->apply();
		meshes2d.texquad->draw();
	}

	void Graphics3D::draw(const Image * image, const SqRect & rect)
	{
		if(image->graphics() != this)
			return;

		image->apply();
		setArea(rect);

		techniques2d.image->apply();
		meshes2d.texquad->draw();
	}

	void Graphics3D::draw(const Symbol * symbol, int x, int y)
	{
		auto image = symbol->image();

		if(image == nullptr || image->graphics() != this)
			return;

		image->apply();
		setArea(IntRect({x + symbol->left(), y + symbol->top()}, {image->width(), image->height()}));

		techniques2d.text->apply();
		meshes2d.texquad->draw();
	}

	VertexLayout * Graphics3D::getVertexLayout(const string & fingerprint)
	{
		auto & layout = _vertexLayouts[fingerprint];

		if(layout == nullptr)
			layout.init(this, fingerprint);

		return layout;
	}

	VertexElement * Graphics3D::getVertexElement(const string & key)
	{
		return _vertexElements[key];
	}

	void Graphics3D::addShaderPrograms(const RawShaderCode * codeSets, size_t count)
	{
		for(size_t i = 0; i < count; ++i)
		{
			auto & shader = codeSets[i];
			ShaderCodeSet codeSet;

			for(int j = 0; j < shader.count; ++j)
			{
				auto & unit = shader.units[j];
				codeSet.insert({ShaderType(unit.type), handle<ShaderCode>(unit.data, unit.size)});
			}

			addShaderProgram(shader.id, getVertexLayout(shader.layout), codeSet);
		}
	}

	const Handle<ShaderProgram> & Graphics3D::getShaderProgram(const string & id)
	{
		auto & program = _shaderPrograms[id];

		if(program == nullptr)
			throw Exception("Can't find program with id: ", id);

		return program;
	}

	void Graphics3D::registerVertexElement(const string & key, const string & semantic, uint units)
	{
		_vertexElements[key].init(key, semantic, units);
	}

	void Graphics3D::initFacilities()
	{
		auto p2   = getVertexLayout("p2");
		auto p2t  = getVertexLayout("p2 t");
		auto p3   = getVertexLayout("p3");
		auto p3t  = getVertexLayout("p3 t");
		auto p3c4 = getVertexLayout("p3 c4");
		auto p3n   = getVertexLayout("p3 n");
		auto p3nt  = getVertexLayout("p3 n t");
		auto p3nc4 = getVertexLayout("p3 n c4");

		meshes2d.quad      = createMesh(p2,   VertexData::quad2d);
		meshes2d.texquad   = createMesh(p2t,  VertexData::texquad2d);
		meshes2d.linequad  = createMesh(p2,   VertexData::linequad2d, VertexTopology::Lines);
		meshes3d.quad      = createMesh(p3,   VertexData::quad3d);
		meshes3d.texquad   = createMesh(p3t,  VertexData::texquad3d);
		meshes3d.linequad  = createMesh(p2,   VertexData::linequad3d, VertexTopology::Lines);

		meshes3d.cube			 = createMesh(p3,    VertexData::cube,             VertexIndices::cube);
		meshes3d.texcube		 = createMesh(p3t,   VertexData::texcube,          VertexIndices::cube);
		meshes3d.colorcube		 = createMesh(p3c4,  VertexData::colorcube,        VertexIndices::cube);
		meshes3d.normalcube      = createMesh(p3n,   VertexData::normalcube,       VertexIndices::cube);
		meshes3d.normaltexcube   = createMesh(p3nt,  VertexData::normaltexcube,    VertexIndices::cube);
		meshes3d.normalcolorcube = createMesh(p3nc4, VertexData::normalcolorcube,  VertexIndices::cube);

		meshes3d.linecube = createMesh(p3, VertexData::cube, VertexIndices::linecube, VertexTopology::Lines);


		checkForErrors();

		techniques2d.rectangle.init(getShaderProgram("2d/rect"));
		techniques2d.ellipse  .init(getShaderProgram("2d/ellipse"));
		techniques2d.figure   .init(getShaderProgram("2d/figure"));
		techniques2d.image    .init(getShaderProgram("2d/image"));
		techniques2d.text     .init(getShaderProgram("2d/text"));

		techniques2d.wired_rectangle.init(getShaderProgram("2d/wired/rect"));
		techniques2d.wired_ellipse  .init(getShaderProgram("2d/wired/ellipse"));

		techniques3d.color     .init(getShaderProgram("3d/color"));
		techniques3d.multicolor.init(getShaderProgram("3d/multicolor"));
		techniques3d.texture   .init(getShaderProgram("3d/texture"));

		checkForErrors();

		updateUniform<Uniforms::Model>();
		updateUniform<Uniforms::View>();
		updateUniform<Uniforms::Projection>();

		updateBrushState();

		techniques2d.rectangle->apply();
	}

	void Graphics3D::clearFacilities()
	{
	}

	void Graphics3D::updateBrushState()
	{
		updateUniform<Uniforms::BrushOptions>(color(), lineWidth());
	}

	void Graphics3D::updateSurface()
	{
		clip(_surface->size());
		updateUniform<Uniforms::Viewport>(FloatSize {_surface->size()});
	}

	void Graphics3D::setArea(const IntRect & rect)
	{
		FloatPoint t;
		FloatSize s;

		ScreenCoord::toRel(rect, viewport(), t, s);
		updateUniform<Uniforms::Area>(move(t), move(s), _depth);
	}

	void Graphics3D::setArea(const SqRect & rect)
	{
		FloatPoint t;
		FloatSize s;

		ScreenCoord::toRel(rect, viewport(), t, s);
		updateUniform<Uniforms::Area>(move(t), move(s), _depth);
	}
}

//---------------------------------------------------------------------------