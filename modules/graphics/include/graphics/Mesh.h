//---------------------------------------------------------------------------

#pragma once

#ifndef MESH_H
#define MESH_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <container/Data.h>
#include <core/memory/Pool.h>
#include "VertexLayout.h"

//---------------------------------------------------------------------------

namespace asd
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
		static api(graphics) const VertexData normalcube;
		static api(graphics) const VertexData normaltexcube;
		static api(graphics) const VertexData normalcolorcube;
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
		virtual ~MeshBuffer() {}

		virtual void apply() const = 0;
	};

	class VertexBuffer : public MeshBuffer
	{
	public:
		VertexBuffer(VertexLayout * layout, const VertexData & vd) : layout(layout), verticesCount(static_cast<uint>(vd.size() / layout->units)) {}

		VertexLayout * layout;
		uint verticesCount;
	};

	struct InstancedMeshData;

	struct InstancedMeshDataChunk
	{
		inline InstancedMeshDataChunk(InstancedMeshData * pool, uint address);

		void fill(size_t offset) {}
		template<class U>
		void fill(size_t offset, const Contents<U> & contents);
		template<class H, class ... T, useif<sizeof...(T) != 0>>
		void fill(size_t offset, const Contents<H> & contents, const Contents<T> &... other);

		InstancedMeshData * pool;
		uint address;
	};

	struct InstancedMeshData
	{
		InstancedMeshData(uint stride) : stride(stride), contents(stride * 16)
		{
			Memory<byte>::fill(contents.ptr, 0, stride * 16);
		}

		~InstancedMeshData() {}

		InstancedMeshDataChunk * makeInstance()
		{
			auto * instance = instances.acquire();

			if(instance != nullptr)
				return instance;

			if(currentOffset + stride > contents.size)
				contents.realloc(contents.size * 2);

			instance = instances.create(this, currentOffset);
			currentOffset += stride;

			return instance;
		}

		void releaseInstance(InstancedMeshDataChunk * instance)
		{
			if(instance->pool != this)
				return;

			Memory<byte>::fill(contents.ptr + instance->address, 0, stride);
			instances.free(instance);
		}

		void clear()
		{
			Memory<byte>::fill(contents.ptr, 0, instances.count() * stride);
			instances.freeAll();
		}

		owned_data<byte> contents;
		uint stride;
		uint currentOffset = 0;
		DataPool<InstancedMeshDataChunk, 0x100> instances;
	};

	struct Position2Attr {};
	struct Position3Attr {};
	struct ColorAttr {};

	declare_contents(Position2Attr, (float2, position));
	declare_contents(Position3Attr, (float3, position));
	declare_contents(ColorAttr, (colorf, color));

	template<class ... U>
	struct MeshInstance
	{
		static void fill(InstancedMeshDataChunk * chunk, const Contents<U> &... contents)
		{
			chunk->fill(0, contents...);
		}
	};

	InstancedMeshDataChunk::InstancedMeshDataChunk(InstancedMeshData * pool, uint address) : pool(pool), address(address) {}

	template<class U>
	void InstancedMeshDataChunk::fill(size_t offset, const Contents<U> & contents)
	{
		Memory<void>::move(pool->contents.ptr + address + offset, contents.pointer(), sizeof(Contents<U>));
	}

	template<class H, class ... T, used_t>
	void InstancedMeshDataChunk::fill(size_t offset, const Contents<H> & contents, const Contents<T> &... other)
	{
		fill(offset, contents);
		fill(offset + sizeof(Contents<H>), other...);
	}

	enum class MeshState
	{
		Ready     = 1,
		Indexed   = 2,
		Instanced = 4
	};

	adapt_enum_flags(MeshState);

	class MeshBuilder : public Shared
	{
	public:
		virtual ~MeshBuilder() {}

		virtual MeshBuilder * buffer(const Handle<VertexBuffer> & buffer) = 0;
		virtual MeshBuilder * indices(const VertexIndices & indices) = 0;
		virtual MeshBuilder * offset(uint offset) = 0;
		virtual MeshBuilder * topology(VertexTopology topology) = 0;
		virtual MeshBuilder * makeInstanced(VertexLayout * layout) = 0;

		virtual Handle<Mesh> ready() = 0;

	protected:
		uint _state = 0;
	};

	class Mesh : public Shared
	{
	public:
		virtual ~Mesh() {}

		virtual void draw() const = 0;

		virtual void updateInstanceData() const {}

		virtual InstancedMeshDataChunk * instance()
		{
			return nullptr;
		}

		virtual void remove(InstancedMeshDataChunk *) {}
		virtual void removeInstances() {}
	};
}

//---------------------------------------------------------------------------
#endif
