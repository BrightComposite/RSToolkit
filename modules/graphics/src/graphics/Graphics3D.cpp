//---------------------------------------------------------------------------

#include <graphics/Graphics3D.h>

//---------------------------------------------------------------------------

namespace Rapture
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

	VertexElement VertexElement::pos2("p2", VertexElement::Position, 0, 2);
	VertexElement VertexElement::pos3("p3", VertexElement::Position, 0, 3);
	VertexElement VertexElement::color3("c3", VertexElement::Color, 0, 3);
	VertexElement VertexElement::color4("c4", VertexElement::Color, 0, 4);
	VertexElement VertexElement::tex("t", VertexElement::Texcoord, 0, 2);
	VertexElement VertexElement::normal("n", VertexElement::Normal, 0, 3);
	VertexElement VertexElement::secondaryColor3("s3", VertexElement::Color, 1, 3);
	VertexElement VertexElement::secondaryColor4("s4", VertexElement::Color, 1, 4);

	void VertexElement::writeKey(String & out, VertexElement::Type type, uint units)
	{
		String id(type);
		id << units;

		if(!VertexElement::has(id))
			throw Exception("There is no vertex element with such signature: ", id);

		out << id;
	}

	void UniformData::apply() const
	{
		adapter->bind(*this);
	}

	void MeshInstance::setData(void * data)
	{
		mesh->setData(index, data);
	}

	static Handle<Mesh> createMesh(Graphics3D * graphics, const FigureData & data)
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

		updateAreaUniform(rect);

		if(fm.old == FillMode::Solid)
			techniques2d.rectangle->apply();
		else
			techniques2d.wired_rectangle->apply();

		meshes2d.quad->draw();
	}

	void Graphics3D::ellipse(const IntRect & rect)
	{
		auto fm = hold(_fillMode, FillMode::Solid);

		updateAreaUniform(rect);

		if(fm.old == FillMode::Solid)
			techniques2d.ellipse->apply();
		else
			techniques2d.wired_ellipse->apply();

		meshes2d.texquad->draw();
	}

	void Graphics3D::rectangle(const SqRect & rect)
	{
		auto fm = hold(_fillMode, FillMode::Solid);

		updateAreaUniform(rect);

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
		updateAreaUniform(rect);

		techniques2d.image->apply();
		meshes2d.texquad->draw();
	}

	void Graphics3D::draw(const Image * image, const SqRect & rect)
	{
		if(image->graphics() != this)
			return;

		image->apply();
		updateAreaUniform(rect);

		techniques2d.image->apply();
		meshes2d.texquad->draw();
	}

	void Graphics3D::draw(const Symbol * symbol, int x, int y)
	{
		auto image = symbol->image();

		if(image == nullptr || image->graphics() != this)
			return;

		image->apply();
		updateAreaUniform(IntRect({x + symbol->left(), y + symbol->top()}, {image->width(), image->height()}));

		techniques2d.text->apply();
		meshes2d.texquad->draw();
	}

	VertexLayout * Graphics3D::getVertexLayout(const string & fingerprint)
	{
		auto & layout = _vertexLayouts[fingerprint];

		if(layout == nullptr)
			layout = createVertexLayout(fingerprint);

		return layout;
	}

	const Handle<ShaderProgram> & Graphics3D::getShaderProgram(const string & id)
	{
		auto & program = _shaderPrograms[id];

		if(program == nullptr)
			throw Exception("Can't find program with id: ", id);

		return program;
	}

	void Graphics3D::initFacilities()
	{
		auto p2   = getVertexLayout("p2");
		auto p2t  = getVertexLayout("p2 t");
		auto p3   = getVertexLayout("p3");
		auto p3t  = getVertexLayout("p3 t");
		auto p3c4 = getVertexLayout("p3 c4");

		meshes2d.quad      = createMesh(p2,   VertexData::quad2d);
		meshes2d.texquad   = createMesh(p2t,  VertexData::texquad2d);
		meshes2d.linequad  = createMesh(p2,   VertexData::linequad2d, VertexTopology::Lines);
		meshes3d.quad      = createMesh(p3,   VertexData::quad3d);
		meshes3d.texquad   = createMesh(p3t,  VertexData::texquad3d);
		meshes3d.linequad  = createMesh(p2,   VertexData::linequad3d, VertexTopology::Lines);
		meshes3d.texcube   = createMesh(p3t,  VertexData::texcube,    VertexIndices::cube);
		meshes3d.colorcube = createMesh(p3c4, VertexData::colorcube,  VertexIndices::cube);

		auto cubeVB = createVertexBuffer(p3, VertexData::cube);

		meshes3d.cube      = createMesh(cubeVB, VertexIndices::cube);
		meshes3d.linecube  = createMesh(cubeVB, VertexIndices::linecube, VertexTopology::Lines);

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

	void Graphics3D::updateAreaUniform(const IntRect & rect)
	{
		FloatPoint t;
		FloatSize s;

		ScreenCoord::toRel(rect, viewport(), t, s);
		updateUniform<Uniforms::Area>(move(t), move(s), _depth);
	}

	void Graphics3D::updateAreaUniform(const SqRect & rect)
	{
		FloatPoint t;
		FloatSize s;

		ScreenCoord::toRel(rect, viewport(), t, s);
		updateUniform<Uniforms::Area>(move(t), move(s), _depth);
	}
}

//---------------------------------------------------------------------------