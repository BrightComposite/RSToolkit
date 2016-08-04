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
			Texcoord = 3,
			Max
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

		static api(graphics) void writeKey(String & out, Type type, uint units);

	protected:
		VertexElement(const string & id, Type type, uint index, uint units) : Precached<string, VertexElement>(id),
			id(id), type(type), index(index), units(units) {}
	};

	inline void print(String & s, VertexElement::Type type)
	{
		switch(type)
		{
			case VertexElement::Position:
				s << "p";
				return;
			case VertexElement::Color:
				s << "c";
				return;
			case VertexElement::Normal:
				s << "n";
				return;
			case VertexElement::Texcoord:
				s << "t";
				return;
			default:
				s << "?";
				return;
		}
	}

//---------------------------------------------------------------------------

	class VertexLayout : public Shared
	{
	public:
		api(graphics) VertexLayout(const string & fingerprint);

		virtual void apply() {}
		virtual void accept(const ShaderCode *) {}

		string fingerprint;
		array_list<VertexElement *> elements;
		uint units;
		uint stride;
	};
}

//---------------------------------------------------------------------------
#endif
