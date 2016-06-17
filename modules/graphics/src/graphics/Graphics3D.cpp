//---------------------------------------------------------------------------

#include <graphics/Graphics3D.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(Graphics3D);

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
		/* Front  */ -1.0, -1.0,  1.0, /**/  1.0, -1.0,  1.0, /**/  1.0,  1.0,  1.0, /**/ -1.0,  1.0,  1.0,
		/*  Back  */ -1.0, -1.0, -1.0, /**/ -1.0,  1.0, -1.0, /**/  1.0,  1.0, -1.0, /**/  1.0, -1.0, -1.0,

		/*  Top   */ -1.0,  1.0, -1.0, /**/ -1.0,  1.0,  1.0, /**/  1.0,  1.0,  1.0, /**/  1.0,  1.0, -1.0,
		/* Bottom */ -1.0, -1.0, -1.0, /**/  1.0, -1.0, -1.0, /**/  1.0, -1.0,  1.0, /**/ -1.0, -1.0,  1.0,

		/* Right  */  1.0, -1.0, -1.0, /**/  1.0,  1.0, -1.0, /**/  1.0,  1.0,  1.0, /**/  1.0, -1.0,  1.0,
		/*  Left  */ -1.0, -1.0, -1.0, /**/ -1.0, -1.0,  1.0, /**/ -1.0,  1.0,  1.0, /**/ -1.0,  1.0, -1.0,
	};

	static const float texcubeData[] =
	{
		/* Front  */ -1.0, -1.0,  1.0,   1.0,  1.0, /**/  1.0, -1.0,  1.0,   0.0,  1.0, /**/  1.0,  1.0,  1.0,   0.0,  0.0, /**/ -1.0,  1.0,  1.0,   1.0,  0.0,
		/*  Back  */ -1.0, -1.0, -1.0,   0.0,  1.0, /**/ -1.0,  1.0, -1.0,   0.0,  0.0, /**/  1.0,  1.0, -1.0,   1.0,  0.0, /**/  1.0, -1.0, -1.0,   1.0,  1.0,

		/*  Top   */ -1.0,  1.0, -1.0,   0.0,  1.0, /**/ -1.0,  1.0,  1.0,   0.0,  0.0, /**/  1.0,  1.0,  1.0,   1.0,  1.0, /**/  1.0,  1.0, -1.0,   1.0,  0.0,
		/* Bottom */ -1.0, -1.0, -1.0,   0.0,  0.0, /**/  1.0, -1.0, -1.0,   1.0,  0.0, /**/  1.0, -1.0,  1.0,   1.0,  1.0, /**/ -1.0, -1.0,  1.0,   0.0,  1.0,

		/* Right  */  1.0, -1.0, -1.0,   0.0,  1.0, /**/  1.0,  1.0, -1.0,   0.0,  0.0, /**/  1.0,  1.0,  1.0,   1.0,  0.0, /**/  1.0, -1.0,  1.0,   1.0,  1.0,
		/*  Left  */ -1.0, -1.0, -1.0,   1.0,  1.0, /**/ -1.0, -1.0,  1.0,   0.0,  1.0, /**/ -1.0,  1.0,  1.0,   0.0,  0.0, /**/ -1.0,  1.0, -1.0,   1.0,  0.0
	};

	static const float colorcubeData[] =
	{
		/* Front  */ -1.0, -1.0,  1.0,   0.0,  0.0,  1.0,  1.0, /**/  1.0, -1.0,  1.0,   0.0,  0.0,  1.0,  1.0, /**/  1.0,  1.0,  1.0,   0.0,  0.0,  1.0,  1.0, /**/ -1.0,  1.0,  1.0,   0.0,  0.0,  1.0,  1.0,
		/*  Back  */ -1.0, -1.0, -1.0,   1.0,  1.0,  0.0,  1.0, /**/ -1.0,  1.0, -1.0,   1.0,  1.0,  0.0,  1.0, /**/  1.0,  1.0, -1.0,   1.0,  1.0,  0.0,  1.0, /**/  1.0, -1.0, -1.0,   1.0,  1.0,  0.0,  1.0,

		/*  Top   */ -1.0,  1.0, -1.0,   0.0,  1.0,  0.0,  1.0, /**/ -1.0,  1.0,  1.0,   0.0,  1.0,  0.0,  1.0, /**/  1.0,  1.0,  1.0,   0.0,  1.0,  0.0,  1.0, /**/  1.0,  1.0, -1.0,   0.0,  1.0,  0.0,  1.0,
		/* Bottom */ -1.0, -1.0, -1.0,   1.0,  0.0,  1.0,  1.0, /**/  1.0, -1.0, -1.0,   1.0,  0.0,  1.0,  1.0, /**/  1.0, -1.0,  1.0,   1.0,  0.0,  1.0,  1.0, /**/ -1.0, -1.0,  1.0,   1.0,  0.0,  1.0,  1.0,

		/* Right  */  1.0, -1.0, -1.0,   1.0,  0.0,  0.0,  1.0, /**/  1.0,  1.0, -1.0,   1.0,  0.0,  0.0,  1.0, /**/  1.0,  1.0,  1.0,   1.0,  0.0,  0.0,  1.0, /**/  1.0, -1.0,  1.0,   1.0,  0.0,  0.0,  1.0,
		/*  Left  */ -1.0, -1.0, -1.0,   0.0,  1.0,  1.0,  1.0, /**/ -1.0, -1.0,  1.0,   0.0,  1.0,  1.0,  1.0, /**/ -1.0,  1.0,  1.0,   0.0,  1.0,  1.0,  1.0, /**/ -1.0,  1.0, -1.0,   0.0,  1.0,  1.0,  1.0
	};

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

	VertexData VertexData::quad(quadData);
	VertexData VertexData::texquad(texquadData);
	VertexData VertexData::cube(cubeData);
	VertexData VertexData::texcube(texcubeData);
	VertexData VertexData::colorcube(colorcubeData);

	VertexElement VertexElement::pos2("p2", VertexElement::Position, 0, 2);
	VertexElement VertexElement::pos3("p3", VertexElement::Position, 0, 3);
	VertexElement VertexElement::color3("c3", VertexElement::Color, 0, 3);
	VertexElement VertexElement::colorf("c4", VertexElement::Color, 0, 4);
	VertexElement VertexElement::tex("t", VertexElement::Texcoord, 0, 2);
	VertexElement VertexElement::normal("n", VertexElement::Normal, 0, 3);
	VertexElement VertexElement::secondaryColor3("s3", VertexElement::Color, 1, 3);
	VertexElement VertexElement::secondaryColor4("s4", VertexElement::Color, 1, 4);

	void GraphicModel::draw(int pass) const
	{
		graphics->updateUniform<Uniforms::Model>(getTransform().output());

		technique->apply(pass);
		mesh->draw();
	}

	void Mesh::draw() const
	{
		graphics->bind(vbuffer);
		vbuffer->draw(this);
	}

	void IndexedMesh::draw() const
	{
		graphics->bind(vbuffer);
		graphics->bind(ibuffer);
		ibuffer->draw(this);
	}

	static Handle<Mesh> createMesh(Graphics3D * graphics, const FigureData & data)
	{
		array_list<FloatPoint> points;
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

	void Graphics3D::bind(const Handle<Texture> & texture, uint index)
	{
		if(_textures.size() > index && _textures[index] == texture)
			return;

		_textures[index] = texture;
		texture->apply();
	}

	void Graphics3D::bind(const VertexShader * shader)
	{
		if(_vshader != shader)
		{
			_vshader = shader;
			_vshader->apply();
		}
	}

	void Graphics3D::bind(const PixelShader * shader)
	{
		if(_pshader != shader)
		{
			_pshader = shader;
			_pshader->apply();
		}
	}

	void Graphics3D::bind(const VertexBuffer * buffer)
	{
		if(_vbuffer != buffer)
		{
			_vbuffer = buffer;
			_vbuffer->apply();
		}
	}

	void Graphics3D::bind(const IndexBuffer * buffer)
	{
		if(_ibuffer != buffer)
		{
			_ibuffer = buffer;
			_ibuffer->apply();
		}
	}

	void Graphics3D::rectangle(const IntRect & rect)
	{
		auto fm = hold(_fillMode, FillMode::Solid);

		updateAreaUniform(rect);

		if(fm.old == FillMode::Solid)
			techniques2d.rectangle->apply();
		else
			techniques2d.wired_rectangle->apply();

		meshes.quad->draw();
	}

	void Graphics3D::ellipse(const IntRect & rect)
	{
		auto fm = hold(_fillMode, FillMode::Solid);

		updateAreaUniform(rect);

		if(fm.old == FillMode::Solid)
			techniques2d.ellipse->apply();
		else
			techniques2d.wired_ellipse->apply();

		meshes.texquad->draw();
	}

	void Graphics3D::rectangle(const SqRect & rect)
	{
		auto fm = hold(_fillMode, FillMode::Solid);

		updateAreaUniform(rect);

		if(fm.old == FillMode::Solid)
			techniques2d.rectangle->apply();
		else
			techniques2d.wired_rectangle->apply();

		meshes.quad->draw();
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

		meshes.texquad->draw();
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

	void Graphics3D::draw(const Image * image, int x, int y)
	{
		draw(image, IntRect {x, y, x + image->width(), y + image->height()});
	}

	void Graphics3D::draw(const Image * image, int x, int y, int width, int height)
	{
		draw(image, IntRect {x, y, x + width, y + height});
	}

	void Graphics3D::draw(const Image * image, const IntRect & rect)
	{
		if(image->graphics() != this)
			return;

		image->apply();
		updateAreaUniform(rect);

		techniques2d.image->apply();
		meshes.texquad->draw();
	}

	void Graphics3D::draw(const Image * image, const SqRect & rect)
	{
		if(image->graphics() != this)
			return;

		image->apply();
		updateAreaUniform(rect);

		techniques2d.image->apply();
		meshes.texquad->draw();
	}

	void Graphics3D::draw(const Symbol * symbol, int x, int y)
	{
		if(symbol->graphics() != this)
			return;

		auto image = symbol->image();

		if(image == nullptr)
			return;

		image->apply();

		updateAreaUniform(IntRect({x + symbol->left(), y + symbol->top()}, {image->width(), image->height()}));

		techniques2d.text->apply();
		meshes.texquad->draw();
	}

	const Handle<ShaderCode> & Graphics3D::getShaderCode(const string & id, ShaderType type) const
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

	const Handle<ShaderProgram> & Graphics3D::getShaderProgram(const string & id)
	{
		return shaderPrograms[id];
	}

	Handle<Mesh> Graphics3D::createMesh(VertexLayout * layout, const VertexData & data)
	{
		return Handle<Mesh>(this, createVertexBuffer(layout, data), layout->stride, data.start);
	}

	Handle<Mesh> Graphics3D::createMesh(const string & fingerprint, const VertexData & data)
	{
		auto layout = getVertexLayout(fingerprint);
		return Handle<Mesh>(this, createVertexBuffer(layout, data), layout->stride, data.start);
	}

	Handle<IndexedMesh> Graphics3D::createIndexedMesh(VertexLayout * layout, const VertexData & data, const VertexIndices & indices, uint indicesLocation)
	{
		return Handle<IndexedMesh>(this, createVertexBuffer(layout, data), createIndexBuffer(indices), layout->stride, data.start, indicesLocation);
	}

	void Graphics3D::initFacilities()
	{
		auto p2 = getVertexLayout("p2");
		auto p2t = getVertexLayout("p2 t");
		auto p3 = getVertexLayout("p3");
		auto p3t = getVertexLayout("p3 t");
		auto p3c4 = getVertexLayout("p3 c4");

		meshes.quad = createMesh(p2, VertexData::quad);
		meshes.texquad = createMesh(p2t, VertexData::texquad);
		meshes.cube = createIndexedMesh(p3, VertexData::cube, VertexIndices::cube);
		meshes.texcube = createIndexedMesh(p3t, VertexData::texcube, VertexIndices::cube);
		meshes.colorcube = createIndexedMesh(p3c4, VertexData::colorcube, VertexIndices::cube);

		techniques2d.rectangle.init(getShaderProgram("2d/rect"));
		techniques2d.ellipse.init(getShaderProgram("2d/ellipse"));
		techniques2d.wired_rectangle.init(getShaderProgram("2d/wired/rect"));
		techniques2d.wired_ellipse.init(getShaderProgram("2d/wired/ellipse"));
		techniques2d.figure.init(getShaderProgram("2d/figure"));
		techniques2d.image.init(getShaderProgram("2d/image"));
		techniques2d.text.init(getShaderProgram("2d/text"));

		techniques3d.color.init(getShaderProgram("3d/color"));
		techniques3d.multicolor.init(getShaderProgram("3d/multicolor"));
		techniques3d.texture.init(getShaderProgram("3d/texture"));

		updateUniform<Uniforms::Model>();
		updateUniform<Uniforms::View>();
		updateUniform<Uniforms::Projection>();
		updateBrushState();

		techniques2d.rectangle->apply();
	}

	void Graphics3D::updateBrushState()
	{
		updateUniform<Uniforms::BrushOptions>(color(), lineWidth());
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