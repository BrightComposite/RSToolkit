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
	class InstancedMesh;

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

	class Mesh : public Shared
	{
	public:
		Mesh(ArrayList<MeshBuffer> && buffers, uint verticesCount, uint verticesLocation) : buffers(move(buffers)), verticesLocation(verticesLocation), verticesCount(verticesCount) {}

		virtual api(graphics) void draw() const = 0;

	protected:
		ArrayList<MeshBuffer> buffers;
		VertexLayout * layout = nullptr;

		uint verticesLocation;
		uint verticesCount;
	};

	struct MeshInputLayout
	{

		size_t size;
	};

	class MeshInstance : public Shared
	{
	public:
		MeshInstance(InstancedMesh * mesh, uint index) : mesh(mesh), index(index) {}

		api(graphics) void setData(void * data);

		InstancedMesh * mesh;
		uint index;
	};

	class InstancedMesh : public Mesh
	{
	public:
		InstancedMesh(MeshInputLayout * instanceLayout, ArrayList<MeshBuffer> && buffers, uint verticesCount, uint verticesLocation) : Mesh(move(buffers), verticesLocation, verticesCount), instanceLayout(instanceLayout) {}

		virtual MeshInstance * createInstance() = 0;
		virtual void setData(uint index, void * data) = 0;

	protected:
		MeshInputLayout * instanceLayout;
		ArrayList<MeshInstance> instances;
	};
}

//---------------------------------------------------------------------------
#endif
