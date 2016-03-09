//---------------------------------------------------------------------------

#ifndef VERTEX_INPUT_LAYOUT_H
#define VERTEX_INPUT_LAYOUT_H

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
	class VertexInputLayout;
	class VertexInputElement;

	typedef VertexInputLayout Vil;
	typedef VertexInputElement Vie;

	class VertexInputElement : public Shareable<VertexInputElement>
	{
		friend VertexInputLayout;

	public:
		string id;
		uint size;

		static inline Handle<VertexInputElement> get(const string & id)
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

		static VertexInputElement pos2;
		static VertexInputElement pos3;
		static VertexInputElement color3;
		static VertexInputElement color4;
		static VertexInputElement secondaryColor3;
		static VertexInputElement secondaryColor4;
		static VertexInputElement tex;
		static VertexInputElement normal;

	protected:
		VertexInputElement(const string & id, uint size) : id(id), size(size)
		{
			pool.add(id, share(*this));
		}

		static Map<string, VertexInputElement> pool;
	};

	class VertexInputLayout : public Shareable<VertexInputLayout>
	{
	public:
		static inline VertexInputLayout & get(const String & id)
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
		Array<VertexInputElement> elements;

	protected:
		VertexInputLayout(const String & key) : key(key)
		{
			stride = decodeData(key, elements);
		}

		static uint decodeData(const String & data, Array<VertexInputElement> & elements)
		{
			uint stride;

			for(const auto & str : split(data))
			{
				auto vie = VertexInputElement::get(*str);
				stride += vie->size;
				elements.addLast(vie);
			}

			return stride;
		}

		static Map<string, VertexInputLayout> pool;
	};
#endif
}

//---------------------------------------------------------------------------
#endif
