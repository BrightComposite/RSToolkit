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

	class VertexBuffer : public Shared
	{
	public:
		VertexBuffer(VertexLayout * layout, const VertexData & vd) : layout(layout), verticesCount(static_cast<uint>(vd.size() / layout->units)) {}

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
		Mesh(const Handle<VertexBuffer> & vbuffer, VertexTopology topology, uint verticesLocation = 0) : vbuffer(vbuffer), verticesLocation(verticesLocation) {}

		virtual api(graphics) void draw() const = 0;

		Handle<VertexBuffer> vbuffer;
		uint verticesLocation;
	};

	class IndexedMesh : public Mesh
	{
	public:
		IndexedMesh(const Handle<VertexBuffer> & vbuffer, const Handle<IndexBuffer> & ibuffer, VertexTopology topology, uint verticesLocation = 0, uint indicesLocation = 0) : Mesh(vbuffer, topology, verticesLocation), ibuffer(ibuffer), indicesLocation(indicesLocation) {}

		Handle<IndexBuffer> ibuffer;
		uint indicesLocation;
	};
}

//---------------------------------------------------------------------------
#endif
