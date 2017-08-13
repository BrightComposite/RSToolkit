//---------------------------------------------------------------------------

#pragma once

#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

//---------------------------------------------------------------------------

#include <core/String.h>
#include <core/Exception.h>

#include <core/addition/cached.h>

#include "Graphics.h"

//---------------------------------------------------------------------------

namespace asd
{
	class Graphics3D;
	class VertexElement;

	class VertexElement : public shareable
	{
		friend class Graphics3D;

	public:
		string key;
		String semantic;
		uint units;

	protected:
		friend_owned_handle(VertexElement);

		VertexElement(const string & key, const string & semantic, uint units) : key(key), semantic(semantic), units(units) {}
	};

//---------------------------------------------------------------------------

	class vertex_layout : public shareable
	{
		friend class Graphics3D;

	public:
		string fingerprint;
		array_list<VertexElement *> elements;
		uint units;
		uint stride;

	protected:
		friend_owned_handle(vertex_layout);

		api(graphics) vertex_layout(Graphics3D * graphics, const string & fingerprint);
	};
}

//---------------------------------------------------------------------------
#endif
