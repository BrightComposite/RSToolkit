//---------------------------------------------------------------------------

#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

//---------------------------------------------------------------------------

#include <core/String.h>
#include <core/Exception.h>

#include <core/container/Array.h>
#include <core/container/Map.h>

#include "Graphics.h"

//---------------------------------------------------------------------------

namespace Rapture
{
#if 0
	class VertexElement : public Shareable<VertexElement>
	{
		friend VertexLayout;

	public:
		string id;
		uint size;

		static inline Handle<VertexElement> get(const string & id)
		{
#ifdef _DEBUG
			auto && h = pool[id];

			if(h == nullptr)
				throw Exception("Can't find vertex input element with key -  \"", id, "\"");

			return h;
#else
			return pool.get(id);
#endif
		}

		static VertexElement pos2;
		static VertexElement pos3;
		static VertexElement color3;
		static VertexElement color4;
		static VertexElement secondaryColor3;
		static VertexElement secondaryColor4;
		static VertexElement tex;
		static VertexElement normal;

	protected:
		VertexElement(const string & id, uint size) : id(id), size(size)
		{
			pool.add(id, share(*this));
		}

		static Map<string, VertexElement> pool;
	};

	class VertexLayout : public Shareable<VertexLayout>
	{
	public:
		static inline VertexLayout & get(const String & id)
		{
			auto && h = pool[id];

			if(h == nullptr)
			{
				h.reinit(id);
				pool.add(id, h);
			}

			return *h;
		}

		string key;
		uint stride;
		Array<VertexElement> elements;

	protected:
		VertexLayout(const String & key) : key(key)
		{
			stride = decodeData(key, elements);
		}

		static uint decodeData(const String & data, Array<VertexElement> & elements)
		{
			uint stride;

			for(const auto & str : split(data))
			{
				auto vie = VertexElement::get(*str);
				stride += vie->size;
				elements.addLast(vie);
			}

			return stride;
		}

		static Map<string, VertexLayout> pool;
	};
#endif
}

//---------------------------------------------------------------------------
#endif
