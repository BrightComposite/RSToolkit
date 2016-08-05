//---------------------------------------------------------------------------

#pragma once

#ifndef MESH_H
#define MESH_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <core/container/Data.h>
#include "VertexLayout.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;
	class Mesh;

	class VertexData : public array_list<float>
	{
	public:
		using array_list<float>::array_list;

		static api(graphics) const VertexData quad2d;
		static api(graphics) const VertexData texquad2d;
		static api(graphics) const VertexData linequad2d;
		static api(graphics) const VertexData quad3d;
		static api(graphics) const VertexData texquad3d;
		static api(graphics) const VertexData linequad3d;
		static api(graphics) const VertexData cube;
		static api(graphics) const VertexData texcube;
		static api(graphics) const VertexData colorcube;
	};

	class VertexIndices : public array_list<uint16_t>
	{
	public:
		using array_list<uint16_t>::array_list;

		static api(graphics) VertexIndices cube;
		static api(graphics) VertexIndices linecube;
	};

	enum class VertexTopology
	{
		Triangles,
		TriangleStrip,
		Lines,
		LineStrip
	};

	class MeshBuffer : public Shared
	{
	public:
		MeshBuffer() {}

		virtual void apply() const = 0;
	};

	class VertexBuffer : public MeshBuffer
	{
	public:
		VertexBuffer(VertexLayout * layout, const VertexData & vd) : layout(layout), verticesCount(static_cast<uint>(vd.size() / layout->units)) {}

		VertexLayout * layout;
		uint verticesCount;
	};

	struct MeshInstance : Shared
	{
		template<class U>
		void fill(const Contents<U> & contents)
		{
			Memory<void>::move(data.ptr, contents.pointer(), data.size);
		}

		data<void> data;
	};

	enum class MeshState
	{
		Ready     = 1,
		Indexed   = 2,
		Instanced = 4
	};

	adapt_enum_flags(MeshState);

	class Mesh : public Shared
	{
	public:
		virtual ~Mesh() {}

		virtual Mesh * buffer(const Handle<VertexBuffer> & buffer) = 0;
		virtual Mesh * indices(const VertexIndices & indices, uint offset = 0) = 0;
		virtual Mesh * topology(VertexTopology topology) = 0;
		virtual Mesh * instanceLayout(VertexLayout * layout) = 0;

		virtual const Mesh * ready() = 0;

		virtual MeshInstance * instance() const = 0;

		virtual void draw() const = 0;

		uint offset() const
		{
			return _verticesOffset;
		}

		Mesh * offset(uint offset)
		{
			_verticesOffset = offset;
			return this;
		}

		uint verticesCount() const
		{
			return _verticesCount;
		}

	protected:
		uint _verticesOffset = 0;
		uint _verticesCount = 0;
		uint _state = 0;
	};
}

//---------------------------------------------------------------------------
#endif
