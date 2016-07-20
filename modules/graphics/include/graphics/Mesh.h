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

	class VertexData : public data<const void>
	{
	public:
		using data<const void>::data;

		static api(graphics) VertexData quad2d;
		static api(graphics) VertexData texquad2d;
		static api(graphics) VertexData linequad2d;
		static api(graphics) VertexData quad3d;
		static api(graphics) VertexData texquad3d;
		static api(graphics) VertexData linequad3d;
		static api(graphics) VertexData cube;
		static api(graphics) VertexData texcube;
		static api(graphics) VertexData colorcube;

		uint start = 0;
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

	class VertexBuffer : public Shared
	{
	public:
		VertexBuffer(VertexLayout * layout, const VertexData & vd) : layout(layout), verticesCount(static_cast<uint>(vd.size / layout->stride)) {}

		virtual void apply() const = 0;

		VertexLayout * layout;
		uint verticesCount;
	};

	class IndexBuffer : public Shared
	{
	public:
		IndexBuffer(const VertexIndices & indices) : size(static_cast<uint>(indices.size())) {}

		virtual void apply() const = 0;

		uint size;
	};

	class Mesh : public Shared
	{
	public:
		Mesh(const Handle<VertexBuffer> & vbuffer, VertexTopology topology, uint stride, uint verticesLocation = 0) : vbuffer(vbuffer), stride(stride), verticesLocation(verticesLocation) {}

		virtual api(graphics) void draw() const = 0;

		Handle<VertexBuffer> vbuffer;
		uint stride, verticesLocation;
	};

	class IndexedMesh : public Mesh
	{
	public:
		IndexedMesh(const Handle<VertexBuffer> & vbuffer, const Handle<IndexBuffer> & ibuffer, VertexTopology topology, uint stride, uint verticesLocation = 0, uint indicesLocation = 0) : Mesh(vbuffer, topology, stride, verticesLocation), ibuffer(ibuffer), indicesLocation(indicesLocation) {}

		Handle<IndexBuffer> ibuffer;
		uint indicesLocation;
	};
}

//---------------------------------------------------------------------------
#endif
