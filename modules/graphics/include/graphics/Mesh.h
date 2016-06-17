//---------------------------------------------------------------------------

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
	class IndexedMesh;

	class VertexData : public data<const void>
	{
	public:
		using data<const void>::data;

		static api(graphics) VertexData quad;
		static api(graphics) VertexData texquad;
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
	};

	class VertexBuffer : public Shared
	{
	public:
		VertexBuffer(VertexLayout * layout, const VertexData & vd) : layout(layout), verticesCount(static_cast<uint>(vd.size / layout->stride)) {}

		virtual void apply() const = 0;
		virtual void draw(const Mesh * mesh) const = 0;

		VertexLayout * layout;
		uint verticesCount;
	};

	class IndexBuffer : public Shared
	{
	public:
		IndexBuffer(const VertexIndices & indices) : size(static_cast<uint>(indices.size())) {}

		virtual void apply() const = 0;
		virtual void draw(const IndexedMesh * mesh) const = 0;

		uint size;
	};

	class Mesh : public Shared
	{
	public:
		Mesh(Graphics3D * graphics, const Handle<VertexBuffer> & vbuffer, uint stride, uint verticesLocation = 0) : graphics(graphics), vbuffer(vbuffer), stride(stride), verticesLocation(verticesLocation) {}

		virtual void api(graphics) draw() const;

		Graphics3D * graphics;
		Handle<VertexBuffer> vbuffer;
		uint stride, verticesLocation;
	};

	class IndexedMesh : public Mesh
	{
	public:
		IndexedMesh(Graphics3D * graphics, const Handle<VertexBuffer> & vbuffer, const Handle<IndexBuffer> & ibuffer, uint stride, uint verticesLocation = 0, uint indicesLocation = 0) : Mesh(graphics, vbuffer, stride, verticesLocation), ibuffer(ibuffer), indicesLocation(indicesLocation) {}

		virtual void api(graphics) draw() const override;

		Handle<IndexBuffer> ibuffer;
		uint indicesLocation;
	};
}

//---------------------------------------------------------------------------
#endif
