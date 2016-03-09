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

	VertexData VertexData::quad(quadData);
	VertexData VertexData::texquad(texquadData);
}

//---------------------------------------------------------------------------
