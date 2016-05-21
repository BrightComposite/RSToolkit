//---------------------------------------------------------------------------

#ifndef MESH_H
#define MESH_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <core/container/RawData.h>
#include "VertexLayout.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;
	class Mesh;
	class IndexedMesh;

	class VertexData : public RawData<const void>
	{
	public:
		using RawData<const void>::RawData;

		static VertexData quad;
		static VertexData texquad;
		static VertexData cube;
		static VertexData texcube;

		uint start = 0;
	};

	class VertexIndices : public vector<uint16_t>
	{
	public:
		using vector<uint16_t>::vector;

		static VertexIndices cube;
	};

	link_class(Mesh, Class<Object>);

	class VertexBuffer : public Shared
	{
	public:
		VertexBuffer(VertexLayout * vil, const VertexData & vd) : vil(vil), verticesCount(static_cast<uint>(vd.size / vil->stride)) {}

		virtual void apply() const = 0;
		virtual void draw(const Mesh * mesh) const = 0;

		VertexLayout * vil;
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
		Mesh(const Handle<VertexBuffer> & vbuffer, uint stride, uint verticesLocation = 0) : vbuffer(vbuffer), verticesLocation(verticesLocation), stride(stride) {}

		virtual void draw(Graphics3D * graphics) const;

		Handle<VertexBuffer> vbuffer;
		uint stride, verticesLocation;
	};

	class IndexedMesh : public Mesh
	{
	public:
		IndexedMesh(const Handle<VertexBuffer> & vbuffer, const Handle<IndexBuffer> & ibuffer, uint stride, uint verticesLocation = 0, uint indeicesLocation = 0) : Mesh(vbuffer, verticesLocation, stride), ibuffer(ibuffer), indicesLocation(indicesLocation) {}

		virtual void draw(Graphics3D * graphics) const override;

		Handle<IndexBuffer> ibuffer;
		uint indicesLocation;
	};
}

//---------------------------------------------------------------------------
#endif
