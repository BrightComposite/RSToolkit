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

	class VertexElement : public Shared, public Precached<string, VertexElement>
	{
		friend class VertexLayout;

	public:
		string id;
		const char * semantic;
		uint index;
		uint units;

		static VertexElement pos2;
		static VertexElement pos3;
		static VertexElement color3;
		static VertexElement colorf;
		static VertexElement secondaryColor3;
		static VertexElement secondaryColor4;
		static VertexElement tex;
		static VertexElement normal;

	protected:
		VertexElement(const string & id, const char * semantic, uint index, uint units) : Precached<string, VertexElement>(id),
			id(id), semantic(semantic), index(index), units(units) {}
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
		vector<VertexElement *> elements;
		uint stride;
	};
}

//---------------------------------------------------------------------------
#endif
