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

namespace Rapture
{
	class ShaderCode;
	class VertexElement;

	api_class(graphics, Precached<string, VertexElement>);

	class VertexElement : public Shared, public Precached<string, VertexElement>
	{
		friend class VertexLayout;

	public:
		enum Type : int
		{
			Position = 0,
			Color    = 1,
			Normal   = 2,
			Texcoord = 3
		};

		string id;
		Type type;
		uint index;
		uint units;

		static api(graphics) VertexElement pos2;
		static api(graphics) VertexElement pos3;
		static api(graphics) VertexElement color3;
		static api(graphics) VertexElement color4;
		static api(graphics) VertexElement secondaryColor3;
		static api(graphics) VertexElement secondaryColor4;
		static api(graphics) VertexElement tex;
		static api(graphics) VertexElement normal;

	protected:
		VertexElement(const string & id, Type type, uint index, uint units) : Precached<string, VertexElement>(id),
			id(id), type(type), index(index), units(units) {}
	};

//---------------------------------------------------------------------------

	class VertexLayout : public Shared
	{
	public:
		VertexLayout(const string & fingerprint) : fingerprint(fingerprint), stride(0)
		{
			for(const auto & key : split(fingerprint))
			{
				auto & vie = VertexElement::get(key);
				elements.push_back(vie);
				stride += vie->units * sizeof(float);
			}
		}

		virtual void apply() {}
		virtual void accept(const ShaderCode *) {}

		string fingerprint;
		array_list<VertexElement *> elements;
		uint stride;
	};
}

//---------------------------------------------------------------------------
#endif
