//---------------------------------------------------------------------------

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

		static VertexElement api(graphics) pos2;
		static VertexElement api(graphics) pos3;
		static VertexElement api(graphics) color3;
		static VertexElement api(graphics) colorf;
		static VertexElement api(graphics) secondaryColor3;
		static VertexElement api(graphics) secondaryColor4;
		static VertexElement api(graphics) tex;
		static VertexElement api(graphics) normal;

	protected:
		VertexElement(const string & id, Type type, uint index, uint units) : Precached<string, VertexElement>(id),
			id(id), type(type), index(index), units(units) {}
	};

	template class api(graphics) Precached<string, VertexElement>;

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
