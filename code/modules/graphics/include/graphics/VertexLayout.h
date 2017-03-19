//---------------------------------------------------------------------------

#pragma once

#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

//---------------------------------------------------------------------------

#include <core/String.h>
#include <core/Exception.h>

#include <core/addition/Cached.h>

#include "Graphics.h"

//---------------------------------------------------------------------------

namespace asd
{
	class Graphics3D;
	class VertexElement;

	class VertexElement : public Shared
	{
		friend class Graphics3D;

	public:
		string key;
		String semantic;
		uint units;

	protected:
		friend_owned_handle(VertexElement, Graphics3D);

		VertexElement(const string & key, const string & semantic, uint units) : key(key), semantic(semantic), units(units) {}
	};

//---------------------------------------------------------------------------

	class VertexLayout : public Shared
	{
		friend class Graphics3D;

	public:
		string fingerprint;
		array_list<VertexElement *> elements;
		uint units;
		uint stride;

	protected:
		friend_owned_handle(VertexLayout, Graphics3D);

		api(graphics) VertexLayout(Graphics3D * graphics, const string & fingerprint);
	};
}

//---------------------------------------------------------------------------
#endif
